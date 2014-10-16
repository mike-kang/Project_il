#include "date.h"
#include <stdio.h>
#ifdef _WIN32
#include "windows.h"
#endif

#ifdef _DEBUG
#ifdef _WIN32
#ifndef __MINGW32__
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#endif
#endif

using namespace tools;
#ifdef _WIN32
char* getString(char *buf, int type)
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  if(type == 0)
    sprintf(buf, "%4d-%02d-%02d %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
  else if(type == 1)  
    sprintf(buf, "%4d-%02d-%02d-%d", st.wYear, st.wMonth, st.wDay, (st.wHour *60+ st.wMinute)*60 + st.wSecond);
  else if(type == 2)  
    sprintf(buf, "%4d%02d%02d_%d%d%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
  return buf;
}
#else
Date::Date(char* date) //data=20140101
{
  char buf[5];

  memcpy(buf, date, 4); buf[4] = '\0'; 
  m_year = atoi(buf);
  memcpy(buf, date+4, 2); buf[2] = '\0'; 
  m_date = atoi(buf);
  memcpy(buf, date+6, 2); buf[2] = '\0'; 
  m_day = atoi(buf);
  
}

#endif



