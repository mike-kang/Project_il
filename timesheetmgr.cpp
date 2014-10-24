#include "timesheetmgr.h"
#include "tools/log.h"
#include "web/webservice.h"
#include "settings.h"
#include <sys/time.h>

#define LOG_TAG "TimeSheetMgr"

using namespace std;

TimeSheetMgr::TimeSheetMgr(Settings* settings, WebService* ws):m_settings(settings), m_ws(ws)
{
  m_sMemcoCd = m_settings->get("App::MEMCO_CD");
  m_sSiteCd = m_settings->get("App::SITE_CD");
  m_sDvLoc = m_settings->get("App::DV_LOC"); // = "0001";
  m_sDvNo = m_settings->get("App::DV_NO"); // = "6";
  m_cInOut = *m_settings->get("App::IN_OUT").c_str();
    
}

TimeSheetMgr::~TimeSheetMgr()
{
  for(list<TimeSheet*>::iterator itr = m_listTS.begin(); itr != m_listTS.end(); itr++)
    delete *itr;
}

TimeSheetMgr::TimeSheet::TimeSheet(string lab_no, char utype, char* img, int img_sz)
  :m_lab_no(lab_no), m_utype(utype), m_photo_img(img), m_imgSz(img_sz)
{
  struct tm _tm;
  time_t t = time(NULL);
  localtime_r(&t, &_tm);
  char buf[64];
  sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday
    , _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  m_time = buf;
}


void TimeSheetMgr::insert(string lab_no, char utype, char* img, int img_sz)
{
  TimeSheet* ts = new TimeSheet(lab_no, utype, img, img_sz);
  m_listTS.push_back(ts);
}

bool TimeSheetMgr::upload()
{
  // 1. send file

  // 2. send list
  for(list<TimeSheet*>::iterator itr = m_listTS.begin(); itr != m_listTS.end(); itr++){
    bool ret = false;
    try{
      ret = m_ws->request_TimeSheetInsertString(m_sMemcoCd.c_str(), m_sSiteCd.c_str(), (*itr)->m_lab_no.c_str(), m_cInOut, m_sDvNo.c_str(), m_sDvLoc.c_str(), (*itr)->m_utype, (*itr)->m_time.c_str(), 
        (*itr)->m_photo_img, (*itr)->m_imgSz, 3000);
    }
    catch(WebService::Except e){
      LOGE("request_TimeSheetInsertString: %s\n", WebService::dump_error(e));
    }
  }
  return true;
}
  

  

