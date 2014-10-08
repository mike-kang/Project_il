#include "timer.h"
#include <iostream>

using namespace tools;

Timer::Timer(int count, void (*cbFunc)(void*), void* clientData):m_count(count), m_cbFunc(cbFunc), m_clientData(clientData), m_active(true)
{
  pthread_create(&m_threadId, NULL, run, this);
  pthread_detach(m_threadId);
}

void Timer::reset(int count)
{

}

void Timer::cancel()
{
  m_active = false;
}

void* Timer::run(void* arg)
{
  Timer* timer = (Timer*)arg;

  while(timer->m_active && timer->m_count)
  {
    sleep(1);
    timer->m_count--;
  }

  if(timer->m_active && !timer->m_count)
    timer->m_cbFunc(timer->m_clientData);

  return NULL;
}



