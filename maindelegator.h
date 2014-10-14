#ifndef _MAINDELEGATOR_HEADER
#define _MAINDELEGATOR_HEADER

#include "tools/queue.h"
#include "tools/thread.h"
#include "tools/event.h"
#include "serialRfid.h"
#include "web/webservice.h"
#ifdef CAMERA
#include "camera/camerastill.h"
#endif

class MainDelegator : public SerialRfid::SerialRfidDataNoti {
public:
  enum Ret {
    RET_SUCCESS,
  };
  virtual void onData(char* buf);
  MainDelegator() ;
  ~MainDelegator(){};


//request
  bool request_processRfidSerialData(char* serialnum, int timelimit);

private:
  void run(); 

  void _processRfidSerialData(void* arg);

  //static void cb_ServerTimeGet(void* arg);
  //void _cb_ServerTimeGet(void* arg);


  Thread<MainDelegator> *m_thread;
  unsigned long m_threadId;
  TEvent<MainDelegator>* m_event;
  tools::Queue<TEvent< MainDelegator> > m_eventQ;
  Condition m_rfid_process_completed;
  Mutex m_rfid_mtx;
  WebService* m_ws;
  SerialRfid* m_serialRfid;  

  //settings
  int m_rfidCheckInterval; //ms
  int m_rfid_processMaxTime; //ms
#ifdef CAMERA  
  CameraStill* m_cameraStill;
  int m_takePictureMaxWaitTime; //sec
  int m_cameraDelayOffTime; //sec
#endif
};




#endif

