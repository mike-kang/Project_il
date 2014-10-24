#ifndef _TIMER_HEADER
#define _TIMER_HEADER

#include <pthread.h>
#include "mutex.h"
#include "condition.h"

namespace tools {
class Timer {
public:
  Timer(int count, void (*cbFunc)(void*), void* clientData, bool repeat=false);
  ~Timer(){};
  bool IsActive();
  void start();
  //void reset(int count);
  void stop();

private:
  int m_count; //sec
  void (*m_cbFunc)(void*);
  pthread_t m_threadId;
  void* m_clientData;
  bool m_repeat;
  bool m_active;
  static void* run(void* arg);
  Mutex mtx;
  Condition timerCancel;
};


}
#endif //_TIMER_HEADER
