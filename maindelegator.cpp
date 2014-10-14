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
  char* imgBuf = NULL;;
  int imgLength;
  //request
  //int fd = request_processRfidSerialData(buf, m_rfid_processMaxTime);
  
#ifdef CAMERA
  if(m_cameraStill->takePicture(&imgBuf, &imgLength, m_takePictureMaxWaitTime))
  {
    //for test
    if(imgLength < 0)
      fprintf(stderr, "takePicture error!\n");
    else{
      //LOGV("takePicture %x %d\n", imgBuf, imgLength);
      FILE* fp = fopen("test.jpeg", "wb");
      fwrite(imgBuf, 1, imgLength, fp);
      fclose(fp);
    }
  }
  else{
    LOGE("take Picture fail!!!\n");
  }
#endif  
  system("aplay SoundFiles/ok.wav");  
  
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
  int contentsLength;

  m_rfid_mtx.lock();
/*
  Gpio gpio(17, true);
  gpio.write(true);
*/
  //request to web server



error:  
  m_rfid_process_completed.notify_one();
  
  m_rfid_mtx.unlock();
}


MainDelegator::MainDelegator()
{
  bool ret;
  m_thread = new Thread<MainDelegator>(&MainDelegator::run, this, "MainDelegatorThread");
  LOGV("MainDelegator tid=%lu\n", m_thread->getId());

  /********************************************/
  /* setting values                           */
  m_rfid_processMaxTime = 3000; // 3 sec
  m_rfidCheckInterval = 300; // 300 ms
#ifdef CAMERA  
  m_cameraDelayOffTime = 10 * 60; //10 min
  m_takePictureMaxWaitTime = 2; // 2 sec
#endif



#ifdef CAMERA  
  m_cameraStill = new CameraStill(m_cameraDelayOffTime);
#endif
  m_serialRfid = new SerialRfid1356("/dev/ttyAMA0");
  ret = m_serialRfid->open();
  if(!ret)
    LOGE("SerialRfid open fail!\n");

  m_serialRfid->start(m_rfidCheckInterval, this); //interval=300ms  




  m_ws = new WebService("192.168.0.7", 8080);

}

/*
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
*/

