#include "maindelegator.h"
#include "tools/log.h"
#include "serialRfid1356.h"
#include "gpio.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

using namespace tools;
using namespace std;

#define LOG_TAG "MainDelegator"

void MainDelegator::onData(char* buf)
{
  LOGI("onData %s\n", buf);
  //request
  int fd = request_processRfidSerialData(buf, m_rfid_processMaxTime);
  
  
}

void MainDelegator::run()
{
  while(true)
  {
    //dispatch event
    m_event = m_eventQ.pop();
    
    LOGI("MainDelegator::run: %p\n", m_event);
    if(m_event){
      (this->*(m_event->ev_processFunc))(m_event->ev_data);
      delete m_event;
    }
    else{
      LOGI("Terminate MainDelegator event thread\n");
      break;
    }
  }

}

struct client_data {
  int retval;
  int timelimit;

  client_data(int t):timelimit(t){};
};

struct client_data_Rfid : client_data {
  char* m_serialnum;

  client_data_Rfid(int timelimit, char* serialnum=NULL)
    :client_data(timelimit), m_serialnum(serialnum){} 
};

//blocking function
bool MainDelegator::request_processRfidSerialData(char* serialnum, int timelimit)
{
  bool ret = false;
  LOGV("request_processRfidSerialData\n");
  m_rfid_mtx.lock();
  client_data_Rfid* cd = new client_data_Rfid(timelimit, serialnum);
  TEvent<MainDelegator>* e = new TEvent<MainDelegator>(&MainDelegator::_processRfidSerialData, cd);
  m_eventQ.push(e);

  m_rfid_process_completed.wait(m_rfid_mtx);
  m_rfid_mtx.unlock();

  if(cd->retval == RET_SUCCESS){
    ret = true;
  }

  delete cd;
  return ret;

}

void MainDelegator::_processRfidSerialData(void* arg)
{
  int ret;
  client_data_Rfid* cd = (client_data_Rfid*)arg;
  int sock;
  char buf[4096];
  int readlen;
  char* imgBuf = NULL;;
  int imgLength;
  int contentsLength;

  m_rfid_mtx.lock();
/*
  Gpio gpio(17, true);
  gpio.write(true);
*/
  //request to web server
  sock = m_ws->request_RfidInfoSelect("MC00000003", "ST00000005", cd->m_serialnum, cd->timelimit);  //blocked I/O
  if(sock > 0){

    //header
    readlen = recv(sock, buf, 4096, 0); //for header
    if(readlen > 0){
      LOGV("received: %d\n", readlen);
      //debug start
      std::ofstream oOut("received.txt");
      oOut << buf << endl;
      oOut.close();
      //debug end
      char* p = strstr(buf, " ");
      char* e = strstr(p+1, " "); *e = '\0';
      int retVal = atoi(p+1);
      LOGV("return val: %d\n", retVal);
      if(retVal != 200){
        LOGE("fail ---\n"); 
      }
      p = strstr(e+1, "\nContent-Length:");
      //LOGV("p: %x\n", p);
      p+=17; //"\nContent-Length: " 
      e = strstr(p, "\n"); *e = '\0';
      contentsLength = atoi(p);
      LOGV("length: %d\n", contentsLength);
      LOGV("test: %d\n", e - buf);

      //test
      p = strstr(e+1,"?xml");
      char*p2 = strstr(e+1,"</DataSet>");
      LOGV("test2 %d\n", p2- p);
    }

    //contents
    //char* buffer = new char[contentsLength];
    //readlen = recv(sock, buffer, contentsLength, 0); //for header
    

    
    close(sock);
  }
  else
  {
    LOGE("request_RfidInfoSelect fail!\n");
    goto error;
  }
#ifdef CAMERA
  if(m_cameraStill->takePicture(&imgBuf, &imgLength, m_takePictureMaxWaitTime))
  {
    //for test
    if(imgLength < 0)
      fprintf(stderr, "takePicture error!\n");
    else{
      FILE* fp = fopen("test.jpeg", "wb");
      fwrite(imgBuf, 1, imgLength, fp);
      fclose(fp);
    }
  }
  else{
    LOGE("take Picture fail!!!\n");
  }
#endif  
  system("aplay Sound/ok.wav");  
error:  
  m_rfid_process_completed.notify_one();
  
  m_rfid_mtx.unlock();
}


MainDelegator::MainDelegator()
{
  bool ret;
  m_thread = new Thread<MainDelegator>(&MainDelegator::run, this, "MainDelegatorThread");
  LOGV("MainDelegator tid=%lu\n", m_thread->getId());

  m_rfid_processMaxTime = 3000; // 3 sec

#ifdef CAMERA  
  m_cameraDelayOffTime = 10 * 60; //10 min
  m_takePictureMaxWaitTime = 2; // 2 sec
  m_cameraStill = new CameraStill(m_cameraDelayOffTime);

#endif
  m_serialRfid = new SerialRfid1356("/dev/ttyAMA0");
  ret = m_serialRfid->open();
  if(!ret)
    LOGE("SerialRfid open fail!\n");
  
  m_serialRfid->start(300, this); //interval=300ms  


  m_ws = new WebService("192.168.0.7", 8080);
  m_ws->start();

  int sock;
  sock = m_ws->request_GetNetInfo(3000);  //blocked I/O
  if(sock < 0)
    LOGE("request_GetNetInfo fail!\n");
  char buf[4096+1];
  int readlen;
  if(sock> 0){
    readlen = recv(sock, buf, 4096, 0);
    LOGV("%s\n", buf);
    close(sock);
    
    //debug start
    std::ofstream oOut("received_GetNetInfo.txt");
    oOut << buf << endl;
    oOut.close();
    //debug end
  }
  /*
  sock = m_ws->request_GetNetInfo(3000);  //blocked I/O
  if(sock < 0)
    LOGE("request_GetNetInfo fail!\n");
  char buf[4096];
  int readlen;
  if(sock> 0){
    readlen = recv(sock, buf, 4096, 0);
    LOGV("%s\n", buf);
    close(sock);
    
    //debug start
    std::ofstream oOut("received_GetNetInfo.txt");
    oOut << buf << endl;
    oOut.close();
    //debug end
  }
*/

  sock = m_ws->request_RfidInfoSelectAll("MC00000003", "ST00000005", -1);  //blocked I/O
  if(sock < 0)
    LOGE("request_RfidInfoSelect fail!\n");
  if(sock> 0){
    std::ofstream oOut("received_RfidInfoSelectAll.txt");
    while((readlen = recv(sock, buf, 4096, 0)) > 0){
      LOGV("received: %d\n", readlen);
      
      buf[readlen] ='\0';
      
      //debug start
      oOut << buf;
      //debug end
      //if(readlen < 4096) break;
    }
    oOut.close();

    close(sock);
  }

//#define TEST_SERVER_TIME_GET
//#define TEST_RFID_INFO_SELECT   

#ifdef TEST_SERVER_TIME_GET
  m_ws->request_ServerTimeGet(cb_ServerTimeGet, this);
#endif

#ifdef TEST_RFID_INFO_SELECT
    int fd = request_processRfidSerialData("084042224042", -1);
#endif

  LOGV("MainDelegator ---\n");
  

}

void MainDelegator::cb_ServerTimeGet(void* arg)
{
  LOGV("cb_ServerTimeGet\n");
  WebService::req_data* rd = (WebService::req_data*)arg;
  MainDelegator* md = (MainDelegator*)rd->m_client;
  TEvent<MainDelegator>* e = new TEvent<MainDelegator>(&MainDelegator::_cb_ServerTimeGet, arg);
  md->m_eventQ.push(e);
  return;
}

void MainDelegator::_cb_ServerTimeGet(void* arg)
{
  LOGV("_cb_ServerTimeGet\n");
  WebService::req_data* rd = (WebService::req_data*)arg;
  if(rd->retval != WebService::RET_SUCCESS){
    LOGE("cb_ServerTimeGet fail!\n");
    delete rd;
    return;
  }

  char buf[4096];
  recv(rd->fd, buf, 4096, 0);
  LOGV("%s\n", buf);
  delete rd;
}


