#include "timer.h"
#include <iostream>
#include <errno.h>
using namespace tools;

Timer::Timer(int count, void (*cbFunc)(void*), void* clientData):m_count(count), m_cbFunc(cbFunc), m_clientData(clientData), m_active(false)
{
}

void Timer::start()
{
  m_active = true;
  pthread_create(&m_threadId, NULL, run, this);
  pthread_detach(m_threadId);
}

bool Timer::IsActive()
{
  return m_active;
}


void Timer::cancel()
{
  //std::cout <<"Timer::cancel\n" << std::endl;
  mtx.lock();
  m_active = false;
  timerCancel.notify_one();
  mtx.unlock();
}

//static
void* Timer::run(void* arg)
{
  Timer* timer = (Timer*)arg;
  int count = timer->m_count;
  int ret;
  //std::cout <<"Timer::run\n" << std::endl;
  
  timer->mtx.lock();
  ret = timer->timerCancel.timedwait(timer->mtx, count);
  if(!ret){ //cancel
  }
  else if(ret == ETIMEDOUT){ //expired
    //std::cout << "::timer count " << count << std::endl;
    timer->m_cbFunc(timer->m_clientData);
  }
  
  m_active = false;
  timer->mtx.unlock();
  return NULL;
}



