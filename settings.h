#ifndef _SETTINGS_HEADER
#define _SETTINGS_HEADER

#include <iostream>
#include <map>

class Settings {
public:
  static const int EXCEPTION_NO_EXIST_KEY = 0;
  Settings(const char* filename);
  virtual ~Settings(){}

  std::string get(std::string key);
  bool getBool(std::string key);
  int getInt(std::string key);
  
private:  
  std::map<std::string, std::string> m_mapStr;
  std::map<std::string, bool> m_mapBool;
  std::map<std::string, int> m_mapInt;
};




#endif  //_SETTINGS_HEADER


  

