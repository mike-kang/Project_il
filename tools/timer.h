#ifndef _TIMER_HEADER
#define _TIMER_HEADER

#include <pthread.h>

namespace tools {
class Timer {
public:
  Timer(int count, void (*cbFunc)(void*), void* clientData);
  ~Timer(){};
  void reset(int count);
  void cancel();

private:
  int m_count; //sec
  void (*m_cbFunc)(void*);
  pthread_t m_threadId;
  void* m_clientData;
  bool m_active;
  static void* run(void* arg);
};


}
#endif //_TIMER_HEADER
