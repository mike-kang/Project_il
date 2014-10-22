
#ifndef _EMPLOYEEINFOMGR_HEADER
#define _EMPLOYEEINFOMGR_HEADER

#include "tools/date.h"
#include <vector>
#include <string>

class WebService;
class Settings;

class EmployeeInfoMgr {
public:
  struct EmployeeInfo {
    char serial_number[16]; //rfcard
    std::string in_out_gb;
    char utype;
    tools::Date* ent_co_ymd;
    tools::Date* rtr_co_ymd;
    std::string zone_code;
    //display
    std::string company_name;
    std::string name;
    std::string pin_no;
    EmployeeInfo(): ent_co_ymd(NULL), rtr_co_ymd(NULL){}
    ~EmployeeInfo(){
      if(ent_co_ymd) delete ent_co_ymd;
      if(rtr_co_ymd) delete rtr_co_ymd;
    }
  };
  EmployeeInfoMgr(Settings* settings, WebService* ws);
  virtual ~EmployeeInfoMgr(){}

  bool updateDB();
  bool getInfo(char* serialNumber, EmployeeInfo* ei);
  
private:  
  int fillEmployeeInfoes(char *xml_buf, vector<EmployeeInfo*>& elems);
  bool fillEmployeeInfo(char *xml_buf, EmployeeInfo* ei);
  EmployeeInfo* searchDB(char* serialNumber);

  bool m_bUseLocalDB;
  string m_sMemcoCd; // = "MC00000003";
  string m_sSiteCd; //"ST00000005";
  WebService* m_ws;
  Settings* m_settings;
  std::vector<EmployeeInfo*> m_vectorEmployeeInfo;
};




#endif  //_EMPLOYEEINFOMGR_HEADER


  

