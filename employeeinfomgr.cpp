#include "tools/log.h"
#include "tools/filesystem.h"
#include "employeeinfomgr.h"
#include "web/webservice.h"
#include "settings.h"
#include <fstream>

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
  updateDB();
}

bool EmployeeInfoMgr::updateDB()
{
  if(!filesystem::file_exist(DB_FILE)){
    LOGV("download %s\n", DB_FILE);
    try{
      m_ws->request_RfidInfoSelectAll(m_sMemcoCd.c_str(), m_sSiteCd.c_str(), 8000, DB_FILE);
      LOGV("downloaded %s\n", DB_FILE);
    }
    catch(WebService::Except e){
      LOGE("download %s fail\n", DB_FILE);
      return false;
    }
  }
  
  ifstream infile (DB_FILE, ofstream::binary);
  // get size of file
  infile.seekg (0,infile.end);
  long size = infile.tellg();
  infile.seekg (0);
  // allocate memory for file content
  char* xml_buf = new char[size];
  // read content of infile
  infile.read (xml_buf,size);
  infile.close();
  int num = fillEmployeeInfoes(xml_buf, m_vectorEmployeeInfo);
  cout << "members = " << num << endl;
  delete xml_buf;
  return true;
}

static const int EXCEPTION_NOT_FOUND = 0;
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
  else{
    delete key;
    throw EXCEPTION_NOT_FOUND;
  }
  delete key;
  return ret;
}
  
bool EmployeeInfoMgr::getInfo(char* serialNumber, EmployeeInfo* ei)
{
  //cout << "getInfo" << endl;
  if(m_bUseLocalDB){
    EmployeeInfo* t = searchDB(serialNumber);
    if(!t) return false;
    memcpy(ei, t, sizeof(EmployeeInfo));
    return true;
  }

  bool bNetAvailable = false;
  try{
    bNetAvailable = m_ws->request_GetNetInfo(1000);
  }
  catch(WebService::Except e){
    LOGE("request_GetNetInfo: %s\n", WebService::dump_error(e));
  }
  if(!bNetAvailable){
    EmployeeInfo* t = searchDB(serialNumber);
    if(!t) return false;
    memcpy(ei, t, sizeof(EmployeeInfo));
    return true;
  }
  
  try{
    char* xml_buf = m_ws->request_RfidInfoSelect(m_sMemcoCd.c_str(), m_sSiteCd.c_str(), serialNumber, 3000);
    if(xml_buf){
      bool ret = fillEmployeeInfo(xml_buf, ei);
      delete xml_buf;
      return ret;
    }
    return false;
  }
  catch(WebService::Except e){
    LOGE("request_RfidInfoSelect: %s\n", WebService::dump_error(e));
  }
    
}

int EmployeeInfoMgr::fillEmployeeInfoes(char *xml_buf, vector<EmployeeInfo*>& elems)
{
  char *p = xml_buf;
  char *end;
  //cout << xml_buf << endl;
  //LOGV("***fillEmployeeInfo:%s\n", xml_buf);
  while(p = strstr(p, "<Table")){
    //printf("start %x\n", p);
    end = strstr(p+7, "</Table");
    *end = '\0';
    EmployeeInfo* ei = new EmployeeInfo;

    try {
      ei->pin_no = getData(p, "PIN_NO");
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->name = p = getData(p, "LAB_NM");
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->company_name = p = getData(p, "CO_NM");
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      p = getData(p, "RFID_CAR");
      strcpy(ei->serial_number, p); 
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->in_out_gb = p = getData(p, "IN_OUT_GB");
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->zone_code = p = getData(p, "ZONE_CD");
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->ent_co_ymd = new Date(p = getData(p, "ENT_CO_YMD"));
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->ent_co_ymd = new Date(p = getData(p, "RTR_CO_YMD"));
      p += strlen(p) + 1;
    }
    catch(int e){}
    try {
      ei->utype = *getData(p, "UTYPE");
    }
    catch(int e){}

    elems.push_back(ei);
    
    p = end + 8;
  }
  
  return elems.size();
}

bool EmployeeInfoMgr::fillEmployeeInfo(char *xml_buf, EmployeeInfo* ei)
{
  char *p;
  //cout << xml_buf << endl;
  //LOGV("***fillEmployeeInfo:%s\n", xml_buf);
  if(!(p = strstr(xml_buf, "<Table"))){
    LOGV("The Serial# is not exist!\n");
    return false;
  }
  //cout << "getInfo 1:" << p << endl;
  try {
    ei->pin_no = getData(p, "PIN_NO");
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->name = p = getData(p, "LAB_NM");
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->company_name = p = getData(p, "CO_NM");
    p += strlen(p) + 1;
  }
  catch(int e){}
/*  
  try {
    p = getData(p, "RFID_CAR");
    strcpy(ei->serial_number, p); 
    p += strlen(p) + 1;
  }
  catch(int e){}
*/
  try {
    ei->in_out_gb = p = getData(p, "IN_OUT_GB");
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->zone_code = p = getData(p, "ZONE_CD");
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->ent_co_ymd = new Date(p = getData(p, "ENT_CO_YMD"));
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->ent_co_ymd = new Date(p = getData(p, "RTR_CO_YMD"));
    p += strlen(p) + 1;
  }
  catch(int e){}
  try {
    ei->utype = *getData(p, "UTYPE");
  }
  catch(int e){}

  return true;
}

EmployeeInfoMgr::EmployeeInfo* EmployeeInfoMgr::searchDB(char* serialNumber)
{
  for(vector<EmployeeInfo*>::size_type i=0; i< m_vectorEmployeeInfo.size(); i++)
  {
    cout << m_vectorEmployeeInfo[i]->serial_number << endl;
    if(!strcmp(m_vectorEmployeeInfo[i]->serial_number, serialNumber))
      return m_vectorEmployeeInfo[i];
  }
  return NULL;
} 

