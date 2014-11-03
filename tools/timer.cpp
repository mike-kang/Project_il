#include "timer.h"
#include <iostream>
#include <errno.h>
using namespace tools;

Timer::Timer(void (*cbFunc)(void*), void* clientData):m_cbFunc(cbFunc), m_clientData(clientData), m_active(false) 
{
}

void Timer::start(int count, bool repeat)
{
  start(count, 0, repeat);
}

void Timer::start(int sec, int msec, bool repeat)
{
  m_active = true;
  m_sec = sec;
  m_msec = msec;
  m_repeat = repeat;
  pthread_create(&m_threadId, NULL, run, this);
  pthread_detach(m_threadId);
}

bool Timer::IsActive()
{
  mtx.lock();
  return m_active;
  mtx.unlock();
}


void Timer::stop()
{
  //std::cout <<"Timer::cancel\n" << std::endl;
  mtx.lock();
  m_active = false;
  m_repeat = false;
  timerCancel.notify_one();
  mtx.unlock();
}

//static
void* Timer::run(void* arg)
{
  Timer* timer = (Timer*)arg;
  
  int ret;
  //std::cout <<"Timer::run\n" << std::endl;
  timer->mtx.lock();
  do {
    if(timer->m_msec)
      ret = timer->timerCancel.timedwait(timer->mtx, timer->m_sec, timer->m_msec);
    else
      ret = timer->timerCancel.timedwait(timer->mtx, timer->m_sec);
    if(!ret){ //cancel
    }
    else if(ret == ETIMEDOUT){ //expired
      //std::cout << "::timer count " << count << std::endl;
      timer->m_active = false;
      timer->m_cbFunc(timer->m_clientData);
    }
  } while(timer->m_repeat);

  timer->mtx.unlock();
 
  return NULL;
}



