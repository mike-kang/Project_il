#include "settings.h"
#include "inih_r29/INIReader.h"
#include "tools/log.h"

#define LOG_TAG "Settings"

using namespace std;

#define mapStrInsert(category, key, value) \
  m_mapStr.insert(pair<string, string>(#category"::"#key, #value))
#define mapBoolInsert(category, key, value) \
    m_mapBool.insert(pair<string, bool>(#category"::"#key, value))
#define mapIntInsert(category, key, value) \
      m_mapInt.insert(pair<string, int>(#category"::"#key, value))

#define mapStrInsertFormReader(category, key, value) \
  m_mapStr.insert(pair<string, string>(#category"::"#key, reader.Get(#category, #key, #value)))
#define mapBoolInsertFormReader(category, key, value) \
    m_mapBool.insert(pair<string, bool>(#category"::"#key, reader.GetBoolean(#category, #key, value)))
#define mapIntInsertFormReader(category, key, value) \
      m_mapInt.insert(pair<string, int>(#category"::"#key, reader.GetInteger(#category, #key, value)))

Settings::Settings(const char* filename)
{
  INIReader reader(filename);
  if (reader.ParseError() < 0) {
     cout << "Can't load " << filename << endl;
     //default settings
     //App
     mapStrInsert(App, MEMCO_CD, MC00000003);
     mapStrInsert(App, SITE_CD, ST00000005);
     mapStrInsert(App, DV_LOC, 0001);
     mapStrInsert(App, DV_NO, 6);
     mapBoolInsert(App, LOCAL_DATABASE, false);
     
     //Action
     mapBoolInsert(Action, CAPTURE, true);
     mapBoolInsert(Action, RELAY, true);
     mapBoolInsert(Action, SOUND, true);
     
     //Rfid
     mapStrInsert(Rfid, MODE, 1356M); 
     mapIntInsert(Rfid, CHECK_INTERVAL, 300);  // 300 ms
     
     //Camera
     mapIntInsert(Camera, DELAY_OFF_TIME, 600);  //600 sec
     mapIntInsert(Camera, TAKEPICTURE_MAX_WAIT_TIME, 2);  // 2 sec
     
     //Log
     mapStrInsert(Log, CONSOLE_PATH, /dev/pts/3);

     dump();
     return;
  }

  //App
  mapStrInsertFormReader(App, MEMCO_CD, MC00000003);
  mapStrInsertFormReader(App, SITE_CD, ST00000005);
  mapStrInsertFormReader(App, DV_LOC, 0001);
  mapStrInsertFormReader(App, DV_NO, 6);
  mapBoolInsertFormReader(App, LOCAL_DATABASE, false);
  
  //Action
  mapBoolInsertFormReader(Action, CAPTURE, true);
  mapBoolInsertFormReader(Action, RELAY, true);
  mapBoolInsertFormReader(Action, SOUND, true);

  //Rfid
  mapStrInsertFormReader(Rfid, MODE, 1356M); 
  mapIntInsertFormReader(Rfid, CHECK_INTERVAL, 300);  // 300 ms

  //Camera
  mapIntInsertFormReader(Camera, DELAY_OFF_TIME, 600);  //600 sec
  mapIntInsertFormReader(Camera, TAKEPICTURE_MAX_WAIT_TIME, 2);  // 2 sec

  //Log
  mapStrInsertFormReader(Log, CONSOLE_PATH, /dev/pts/3);

  dump();
}


void Settings::dump()
{
  cout << "Settings contents start ******" << endl;
  for(map<string, string>::iterator iter=m_mapStr.begin(); iter != m_mapStr.end(); ++iter)
    cout << "(" << iter->first << ") " << iter->second << endl;
  for(map<string, int>::iterator iter=m_mapInt.begin(); iter != m_mapInt.end(); ++iter)
    cout << "(" << iter->first << ") " << iter->second << endl;
  for(map<string, bool>::iterator iter=m_mapBool.begin(); iter != m_mapBool.end(); ++iter)
    cout << "(" << iter->first << ") " << ((iter->second)?"true":"false") << endl;
  cout << "Settings contents end ******" << endl;
}

string Settings::get(string key)
{
  map<string, string>::iterator iter = m_mapStr.find(key);
  if(iter == m_mapStr.end()){
    LOGE("%s key is not exist\n", key.c_str());
    throw EXCEPTION_NO_EXIST_KEY;
  }
  return iter->second;
}
bool Settings::getBool(string key)
{ 
  map<string, bool>::iterator iter = m_mapBool.find(key);
  if(iter == m_mapBool.end()){
    LOGE("%s key is not exist\n", key.c_str());
    throw EXCEPTION_NO_EXIST_KEY;
  }
  return iter->second;
}
int Settings::getInt(string key)
{ 
  map<string, int>::iterator iter = m_mapInt.find(key);
  if(iter == m_mapInt.end()){
    LOGE("%s key is not exist\n", key.c_str());
    throw EXCEPTION_NO_EXIST_KEY;
  }
  return iter->second;
}






  

