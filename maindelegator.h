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
#include "tools/wavplayer.h"
#include "employeeinfomgr.h"

class TimeSheetMgr;
class MainDelegator : public SerialRfid::SerialRfidDataNoti {
public:
  enum Exception {
    EXCEPTION_RFID_OPEN_FAIL,
  };
  class EventListener {
  public:
    //virtual void onRFSerialNumber(char* serial) = 0;
    virtual void onMessage(std::string tag, std::string data) = 0;
    virtual void onEmployeeInfo(std::string CoName, std::string Name, std::string PinNo, const unsigned char* img_buf, int img_sz) = 0;
    virtual void onStatus(std::string status) = 0;
  };
  enum Ret {
    RET_SUCCESS,
  };
  virtual void onData(const char* buf);
  virtual void onSameData();
  static MainDelegator* createInstance(EventListener* el);
  ~MainDelegator(); 


//request
  //bool request_processRfidSerialData(char* serialnum, int timelimit);
  void setEventListener(EventListener* el){ m_el = el; }

private:
  static MainDelegator* my;
  MainDelegator(EventListener* el);
  void run(); 

  //void _processRfidSerialData(void* arg);
  string getLocationName();
  bool SettingInit();
  bool checkValidate(EmployeeInfoMgr::EmployeeInfo* ei, string& msg);
  bool checkZone(string& sAuth);
  bool checkDate(Date* start, Date* end, string& msg);
  bool checkNetwork();
  void displayNetworkStatus(bool val);
  bool getSeverTime();
  void setRebootTimer(const char* time_buf);
#ifdef SIMULATOR  
  static void cbTestTimer(void* arg);
  static void test_signal_handler(int signo);
  tools::Timer* mTimerForTest;
  std::string m_test_serial_number;
#endif
  static void cbStatusUpdate(void *client_data, int status, void* ret);
  static void cbTimer(void* arg);
  //static void cb_ServerTimeGet(void* arg);
  //void _cb_ServerTimeGet(void* arg);
  static void cbRebootTimer(void* arg);


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
  tools::Timer* m_timer;  //check network, upload status, download db, upload timesheet
  tools::media::WavPlayer* m_wp;
  EventListener* m_el;
  tools::Timer* m_RebootTimer;


  bool m_bFirstDown;
  bool m_bNetworkAvailable;

  //settings
  //string sLog = "Y";
  bool m_bCapture;
  bool m_bRelay;
  bool m_bSound; //true
  bool m_bDatabase;
  string m_sUrl; 
  string m_sAuthCd; 
  string m_sMemcoCd; // = "MC00000003";
  string m_sSiteCd; //"ST00000005";
  string m_sDvLoc; // = "0001";
  string m_sDvNo; // = "1";
  string m_sInOut; // = "I";
  int m_rfidCheckInterval; //ms
  int m_rfid_processMaxTime; //ms
  string m_sRfidMode; //="1356M";
  string m_sRfid1356Port; // /dev/ttyAMA0
  string m_sRfid900Port; // /dev/ttyUSB0
#ifdef CAMERA  
  CameraStill* m_cameraStill;
  int m_takePictureMaxWaitTime; //sec
  int m_cameraDelayOffTime; //sec
  bool m_bSavePictureFile;
#endif
  string m_sLocalIP;
  string m_sLocalMacAddr;
  string m_sServerURL;
  //string m_consolePath;
  
  bool m_bProcessingRfidData;
  bool m_bTimeAvailable;
  
  //Led
  SwitchGpio* m_yellowLed;
  SwitchGpio* m_blueLed;
  SwitchGpio* m_greenLed;
  SwitchGpio* m_redLed;
  //Relay
  SwitchGpio* m_Relay;
};




#endif

