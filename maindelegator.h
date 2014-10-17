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
#include "hardware/switchgpio.h"
#include "employeeinfomgr.h"
#include "settings.h"
#include "tools/timer.h"

class MainDelegator : public SerialRfid::SerialRfidDataNoti {
public:
  enum Ret {
    RET_SUCCESS,
  };
  virtual void onData(char* buf);
  static MainDelegator* getInstance()
  {
    if(my){
      return my;
    }
    my = new MainDelegator();

    return my;
  }   
  ~MainDelegator(){};


//request
  bool request_processRfidSerialData(char* serialnum, int timelimit);

private:
  static MainDelegator* my;
  MainDelegator();
  void run(); 

  void _processRfidSerialData(void* arg);
  bool SettingInit();
  bool checkValidate(EmployeeInfoMgr::EmployeeInfo* ei);
#ifdef SIMULATOR  
  static void cbTestTimer(void* arg);
  static void test_signal_handler(int signo);
  tools::Timer* mTimerForTest;
#endif
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
  Settings m_settings;
  EmployeeInfoMgr* m_employInfoMrg;
  

  bool m_bFirstDown;
  bool m_bNetworkAvailable;

  //settings
  //string sLog = "Y";
  bool m_bCapture;
  bool m_bRelay;
  bool m_bSound; //true
  bool m_bDatabase;
  const char* m_sMemcoCd; // = "MC00000003";
  const char* m_sSiteCd; //"ST00000005";
  const char* m_sDvLoc; // = "0001";
  const char* m_sDvNo; // = "1";
  const char* m_sInOut; // = "I";
  int m_rfidCheckInterval; //ms
  int m_rfid_processMaxTime; //ms
  const char* m_sRfidMode; //="1356M";
#ifdef CAMERA  
  CameraStill* m_cameraStill;
  int m_takePictureMaxWaitTime; //sec
  int m_cameraDelayOffTime; //sec
#endif

  //Led
  SwitchGpio m_yellowLed;
  SwitchGpio m_blueLed;
  SwitchGpio m_greenLed;
  SwitchGpio m_redLed;
};




#endif

