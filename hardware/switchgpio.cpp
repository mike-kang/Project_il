#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include "switchgpio.h"
#include "tools/log.h"
#include "tools/utils.h"

#define LOG_TAG "SwitchGpio"

using namespace tools;

void SwitchGpio::cbOnTimer(void* arg)
{
  SwitchGpio* my = (SwitchGpio*)arg;
  my->off();
}
void SwitchGpio::cbOnArrayTimer(int i, void* arg)
{
  SwitchGpio* my = (SwitchGpio*)arg;
  if(i & 0x01)
    my->off();
  else
    my->on();
}

void SwitchGpio::on(int t) //ms
{
  m_timer = new Timer(cbOnTimer, this);
  m_timer->start(t/1000, t%1000);
  on();
}

void SwitchGpio::on(int* arr, bool repeat)
{
  m_timer = new Timer(cbOnTimer, this);
  m_timer->start(arr, repeat);
  on();
}



