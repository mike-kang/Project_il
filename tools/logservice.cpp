#include "logservice.h"
#include "property.h"
#include "filesystem.h"
#include <fstream>
#include <errno.h>
#include <stdio.h>
#ifdef _WIN32
#include "windows.h"
#include <share.h>
#else
//#include <time.h>
#include <sys/time.h>
#endif

#ifdef _DEBUG_MEMORY_LEAK
#ifdef _WIN32
#ifndef __MINGW32__
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#endif
#endif

using namespace tools;

LogService *LogService::m_instance = NULL;
//static bool log_enable = false;
static LogService::log_t log_type = LogService::TYPE_NULL; //0: file 1:console 2:debugout(windows only)  3: null
static bool log_file_type_flush_per_line = false; //FILE_TYPE_FLUSH_PER_LINE
#ifdef _WIN32
static HANDLE hOut = INVALID_HANDLE_VALUE;  //console handle
#else
std::ofstream consoleOfstream;
#endif
FILE* fp;

typedef void (*writeFunc)(const char*);

void consoleWrite(const char *buf)
{
#ifdef _WIN32
  DWORD size;
  WriteFile(hOut, buf, strlen(buf), &size, NULL);
#else  
  consoleOfstream << buf;
  consoleOfstream.flush();
#endif
}

void fileWrite(const char *buf)
{
  fputs(buf, fp);
  if(log_file_type_flush_per_line)
    fflush(fp);
}

void debugoutWrite(const char *buf)
{
#ifdef _WIN32
  OutputDebugStringA(buf);
#else  
#endif
}

writeFunc writefunc[4] = {
  NULL
  , fileWrite
  , consoleWrite
  , debugoutWrite
};

LogService* LogService::init(LogService::log_t type, const char* path)
{
  if(type == TYPE_NULL)
    return NULL;

  if(m_instance)
    return m_instance;
  
  m_instance = new LogService;
  m_instance->m_thread = new Thread<LogService>(&LogService::run, m_instance);

  log_type = type;
#ifdef _WIN32
  if(log_type == 0){ //file
    filesystem::dir_create("log");
    
    SYSTEMTIME st;
    GetLocalTime(&st);
    char filename[20];
    sprintf(filename, "log\\%4d-%02d-%02d.log", st.wYear, st.wMonth, st.wDay);
    fp = _fsopen(filename, "a", _SH_DENYWR);
    if(!fp){
	    OutputDebugStringA(strerror(errno));
	  }
    log_file_type_flush_per_line = Property::getPropertyBool("Log", "FILE_TYPE_FLUSH_PER_LINE", false);
    
  }
  else if(log_type == TYPE_CONSOLE){ //console
    AllocConsole();
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  }
#else
  if(log_type == TYPE_CONSOLE){ //console
      consoleOfstream.open(path);
  }

#endif
  return m_instance;
}


LogService::~LogService()
{
  m_EntryQ.exit();
  delete m_thread;
#ifdef _WIN32
  if(log_type == TYPE_FILE){
     fclose(fp);
  }
  else if(log_type == 1){
//    if(hOut != INVALID_HANDLE_VALUE)
//      CloseHandle(hOut);
//    FreeConsole();
  }
#else
  if(log_type == TYPE_CONSOLE){ //console
      consoleOfstream.close();
  }
#endif
}


void LogService::setEnable(bool enable)
{
  //todo
  
}

LogService* LogService::getService()
{
  if(log_type == TYPE_NULL)
    return NULL;
  return m_instance;
}

void LogService::run()
{
  char buf[1024];
  while(1)
  {
    //dispatch event
    Entry *e = m_EntryQ.pop();
    if(e){
      sprintf(buf, "[%s][%lu][%s] %s", e->m_tag, e->m_threadID, e->m_t->toString(), e->m_msg);
      writefunc[log_type](buf);
      delete e;
    }
    else{
      writefunc[log_type]("logService Exit\n");
      break;
    }
  };
}

void LogService::sendMessage(int prio, const char *tag, const char *msg)
{
  unsigned long tid;
#ifdef _WIN32
  SYSTEMTIME st;
  tid = GetCurrentThreadId();
  GetLocalTime(&st);
  Time *t = new Time(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
  tid = pthread_self();
  struct timeval  tv;
  struct tm _tm;
	gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &_tm);
  Time *t = new Time(_tm.tm_hour, _tm.tm_min, _tm.tm_sec, tv.tv_usec/1000, _tm.tm_year, _tm.tm_mon, _tm.tm_mday);
#endif
  Entry *e = new Entry(prio, tag, msg, tid, t);
  m_EntryQ.push(e);
}



