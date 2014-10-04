#ifndef _CONDITION_HEADER
#define _CONDITION_HEADER

#ifdef _WIN32
#include "windows.h"
#else
#include <pthread.h>
#endif

#include <iostream>
using namespace std;

class Condition{
public:
  Condition():m_bWait(false)
  {
#ifdef _WIN32  
    m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(!m_event){
      cout << "CreateEvent fail" << endl;
      return;
    }
#else
    //cout << "Condition +++" << endl;
   
    pthread_cond_init(&m_p, NULL);
    //cout << "Condition ___" << endl;
#endif
    //cout << "CreateEvent success" << endl;
  }

  void wait(Mutex& m)
  {
    //cout << "Consition::wait() ++ " << m.get() << endl;
    m_bWait = true;
#ifdef _WIN32  
    m.unlock();
    WaitForSingleObject(m_event, INFINITE);
    m.lock();
#else
    pthread_cond_wait(&m_p, (pthread_mutex_t*)(m.get()));
#endif
    //cout << "Consition::wait() -- " << m.get() << endl;
  }
  void notify_one()
  {
    //cout << "Consition::notify_one() ++ " << endl;
    if(m_bWait){
      m_bWait = false;
#ifdef _WIN32  
      SetEvent(m_event);
#else
      pthread_cond_signal(&m_p);
#endif
    }
  }
  ~Condition()
  {
#ifdef _WIN32  
    CloseHandle(m_event);
#else
    pthread_cond_destroy(&m_p);
#endif
  }
private:
#ifdef _WIN32
  HANDLE m_event;
#else
  pthread_cond_t m_p;
#endif
  bool m_bWait;
  
};

#endif //_CONDITION_HEADER
