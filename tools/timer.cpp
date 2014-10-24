#include "timer.h"
#include <iostream>
#include <errno.h>
using namespace tools;

Timer::Timer(int count, void (*cbFunc)(void*), void* clientData, bool repeat):m_count(count), m_cbFunc(cbFunc), m_clientData(clientData), m_active(false), m_repeat(repeat)
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
    ret = timer->timerCancel.timedwait(timer->mtx, timer->m_count);
    if(!ret){ //cancel
    }
    else if(ret == ETIMEDOUT){ //expired
      //std::cout << "::timer count " << count << std::endl;
      timer->m_cbFunc(timer->m_clientData);
    }
  } while(timer->m_repeat);

  timer->m_active = false;
  timer->mtx.unlock();
 
  return NULL;
}



