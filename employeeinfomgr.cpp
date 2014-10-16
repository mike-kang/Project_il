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

bool EmployeeInfoMgr::getInfo(char* serialNumber, EmployeeInfo* ei)
{

}

