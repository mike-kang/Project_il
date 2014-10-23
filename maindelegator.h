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
#include "tools/date.h"
#include "settings.h"
#include "tools/timer.h"
#include "employeeinfomgr.h"

class TimeSheetMgr;

class MainDelegator : public SerialRfid::SerialRfidDataNoti {
public:
  class EventListener {
  public:
    virtual void onRFSerialNumber(char* serial) = 0;
    virtual void onMessage(const char* msg) = 0;
  };
  enum Ret {
    RET_SUCCESS,
  };
  virtual void onData(char* buf);
  static MainDelegator* getInstance();
  ~MainDelegator(){}; 


//request
  //bool request_processRfidSerialData(char* serialnum, int timelimit);
  void setEventListener(EventListener* el){ m_el = el; }

private:
  static MainDelegator* my;
  MainDelegator();
  void run(); 

  void _processRfidSerialData(void* arg);
  bool SettingInit();
  bool checkValidate(EmployeeInfoMgr::EmployeeInfo* ei, int* errno);
  bool checkZone(string& sAuth);
  bool checkDate(Date* start, Date* end);
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
  Settings* m_settings;
  EmployeeInfoMgr* m_employInfoMrg;
  TimeSheetMgr* m_timeSheetMgr;
  EventListener* m_el;

  bool m_bFirstDown;
  bool m_bNetworkAvailable;

  //settings
  //string sLog = "Y";
  bool m_bCapture;
  bool m_bRelay;
  bool m_bSound; //true
  bool m_bDatabase;
  string m_sAuthCd; 
  string m_sMemcoCd; // = "MC00000003";
  string m_sSiteCd; //"ST00000005";
  string m_sDvLoc; // = "0001";
  string m_sDvNo; // = "1";
  string m_sInOut; // = "I";
  int m_rfidCheckInterval; //ms
  int m_rfid_processMaxTime; //ms
  string m_sRfidMode; //="1356M";
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

