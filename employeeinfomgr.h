
#ifndef _EMPLOYEEINFOMGR_HEADER
#define _EMPLOYEEINFOMGR_HEADER

#include "tools/date.h"
#include <vector>

class WebService;
class Settings;

class EmployeeInfoMgr {
public:
  struct EmployeeInfo {
    char serial_number[16]; //rfcard
    char in_out_gb[5];
    char utype[2];
    char zone_code[5];
    tools::Date ent_co_ymd;
    tools::Date rtr_co_ymd;
  };
  EmployeeInfoMgr(Settings* settings, WebService* ws);
  virtual ~EmployeeInfoMgr(){}

  bool getInfo(char* serialNumber, EmployeeInfo* ei);
  
private:  
  bool m_bUseLocalDB;
  string m_sMemcoCd; // = "MC00000003";
  string m_sSiteCd; //"ST00000005";
  WebService* m_ws;
  Settings* m_settings;
  std::vector<EmployeeInfo> m_vectorEmployeeInfo;
};




#endif  //_EMPLOYEEINFOMGR_HEADER


  

