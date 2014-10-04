#include "log.h"
#include "logservice.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//typedef unsigned char byte;
#ifdef _DEBUG_MEMORY_LEAK
#ifdef _WIN32
#ifndef __MINGW32__
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#endif
#endif

static LogService *logservice = NULL;
static bool m_enabled = false;

void log_init(int type, char* path)
{
  LogService::init(LogService::TYPE_CONSOLE, NULL);
}

int __log_print(int prio, const char *tag, const char *fmt, ...)
{
  va_list ap;
  char buf[LOG_MSG_SIZE];

  LogService *logservice = LogService::getService();
  if(!logservice)
    return 1;
  
  va_start(ap, fmt);
  vsnprintf(buf, LOG_MSG_SIZE, fmt, ap);
  va_end(ap);

  logservice->sendMessage(prio, tag, buf);

  return 0;
}

