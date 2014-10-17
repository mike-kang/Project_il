#include "tools/log.h"
#include "tools/filesystem.h"
#include "employeeinfomgr.h"
#include "web/webservice.h"
#include "settings.h"

#define LOG_TAG "EmployInfoMgr"

using namespace tools;
using namespace std;

#define DB_FILE "employee.xml"

EmployeeInfoMgr::EmployeeInfoMgr(Settings* settings, WebService* ws): m_settings(settings), m_ws(ws)
{
  try{
    m_bUseLocalDB = settings->getBool("App::LOCAL_DATABASE");
    m_sMemcoCd = m_settings->get("App::MEMCO_CD");
    m_sSiteCd = m_settings->get("App::SITE_CD");
  }
  catch(int e)
  {
    m_bUseLocalDB = false;
    m_sMemcoCd = "MC00000003";
    m_sSiteCd = "ST00000005";
  }
  if(!filesystem::file_exist(DB_FILE)){
    LOGV("download %s\n", DB_FILE);
    try{
      m_ws->request_RfidInfoSelectAll(m_sMemcoCd.c_str(), m_sSiteCd.c_str(), 8000, DB_FILE);
      LOGV("downloaded %s\n", DB_FILE);
    }
    catch(WebService::Except e){
      LOGE("download %s fail\n", DB_FILE);
    }
  }
  
}

char* getData(char* xml_buf, const char* tag)
{
  char* p;
  char* ret = NULL;
  int tag_len = strlen(tag);
  char* key = new char[tag_len + 2]; // <XXXX + NULL
  key[0] = '<';
  strcpy(key+1, tag);
  
  if(p = strstr(xml_buf, key)){
    ret = p+tag_len+2;
    p = strstr(ret, "<");
    *p = '\0';
  }

  delete key;
  
  return ret;
}
  
bool EmployeeInfoMgr::getInfo(char* serialNumber, EmployeeInfo* ei)
{
  cout << "getInfo" << endl;
  if(m_bUseLocalDB){
    ; //todo
    return true;
  }

  bool bNet = m_ws->request_GetNetInfo(1000);
  if(!bNet){
    ; //todo
    return true;
  }
  try{
    char* xml_buf = m_ws->request_RfidInfoSelect(m_sMemcoCd.c_str(), m_sSiteCd.c_str(), serialNumber, 3000);
    if(xml_buf){
      char *p;
      cout << xml_buf << endl;
      //LOGV("***RfidInfoSelect:%s\n", xml_buf);
      if(!(p = strstr(xml_buf, "<Table"))){
        LOGV("The Serial# is not exist!\n");
        return false;
      }
      cout << "getInfo 1:" << p << endl;
      ei->in_out_gb = p = getData(p, "IN_OUT_GB");
      cout << "getInfo 2:" << p << endl;
      ei->zone_code = p = getData(p + strlen(p) + 1, "ZONE_CD");
      cout << "getInfo 3" << endl;
      ei->ent_co_ymd = new Date(p = getData(p + strlen(p) + 1, "ENT_CO_YMD"));
      char* rtr = getData(p, "RTR_CO_YMD");
      if(rtr) 
        ei->rtr_co_ymd = new Date(rtr);
      ei->utype = *getData(p + strlen(p) + 1, "UTYPE");
      
      delete xml_buf;
      return true;
    }
    return false;
  }
  catch(WebService::Except e){
    LOGE("request_RfidInfoSelect: %s\n", WebService::dump_error(e));
  }
  
    
    
    
      

    
}

