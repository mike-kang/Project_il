#include "maindelegator.h"
#include "tools/log.h"
#include "serialRfid1356.h"
#include "libRaspiStill.h"
#include "gpio.h"

using namespace tools;

#define LOG_TAG "MainDelegator"

void MainDelegator::onData(char* buf)
{
  LOGI("onData %s\n", buf);
  //request
  int fd = request_processRfidSerialData(buf, 3000);
  
  
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

struct client_date {
  int retval;
  int timelimit;

  client_date(int t):timelimit(t){};
};

struct client_date_Rfid : client_date {
  char* m_serialnum;

  client_date_Rfid(int timelimit, char* serialnum=NULL)
    :client_date(timelimit), m_serialnum(serialnum){} 
};

//blocking function
bool MainDelegator::request_processRfidSerialData(char* serialnum, int timelimit)
{
  bool ret = false;
  LOGV("request_processRfidSerialData\n");
  client_date_Rfid* cd = new client_date_Rfid(timelimit, serialnum);
  TEvent<MainDelegator>* e = new TEvent<MainDelegator>(&MainDelegator::_processRfidSerialData, cd);
  m_eventQ.push(e);

  m_rfid_mtx.lock();
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
  client_date_Rfid* cd = (client_date_Rfid*)arg;
  int sock;
  char buf[4096];



  Gpio gpio(17, true);
  gpio.write(true);

#ifdef CAMERA  
  LOGI("takePicture +++\n");
  int len = takePicture();
  LOGI("takePicture %d ---\n", len);
  if(len < 0)
    fprintf(stderr, "takePicture error!\n");
  else{
    FILE* fp = fopen("test.jpeg", "wb");
    fwrite(m_faceBuf, 1, len, fp);
    fclose(fp);
  }
#endif  
/*
  sock = m_ws->request_RfidInfoSelect("MC00000003", "ST00000005", cd->m_serialnum, cd->timelimit);  //blocked I/O
  if(sock < 0){
    LOGE("request_RfidInfoSelect fail!\n");
    goto error;
  }
  int readlen;
  while((readlen = recv(sock, buf, 4096, 0)) > 0){
    LOGV("%s", buf);
    memset(buf,0,4096);
  }
  close(sock);
*/  
error:  
  m_rfid_process_completed.notify_one();
}


MainDelegator::MainDelegator()
{
  bool ret;
#ifdef CAMERA  
  m_faceBuf = new char[300* 1024];
  init_raspistill(320, 240, m_faceBuf);
#endif
  m_thread = new Thread<MainDelegator>(&MainDelegator::run, this, "MainDelegatorThread");
  LOGV("MainDelegator \n");

#if 0  
  m_serialRfid = new SerialRfid1356("/dev/ttyAMA0");
  ret = m_serialRfid->open();
  if(!ret)
    LOGE("SerialRfid open fail!\n");
  
  m_serialRfid->start(300, this); //interval=300ms  
#endif

  m_ws = new WebService("192.168.0.7", 8080);
  m_ws->start();

  int sock;
  sock = m_ws->request_GetNetInfo(3000);  //blocked I/O
  if(sock < 0)
    LOGE("request_GetNetInfo fail!\n");
  char buf[4096];
  int readlen;
  if(sock> 0){
    readlen = recv(sock, buf, 4096, 0);
    LOGV("%s\n", buf);
   

    close(sock);
  }

  /*
  sock = m_ws->request_RfidInfoSelectAll("MC00000003", "ST00000005", -1);  //blocked I/O
  if(sock < 0)
    LOGE("request_RfidInfoSelect fail!\n");
  if(sock> 0){
    while((readlen = recv(sock, buf, 4096, 0)) > 0){
      LOGV("%s\n", buf);
    }

    close(sock);
  }
   */ 
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


