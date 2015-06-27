#ifndef _FIFOERVICE_HEADER
#define _FIFOERVICE_HEADER

#include "tools/thread.h"

//#include <iostream>

class FifoService {
public:
  FifoService();
  ~FifoService();
  
  
private:
  void run();
  std::size_t readLine(int fd, char *buffer, std::size_t n);
  
  Thread<FifoService>* m_thread;
  int m_cmdFd;
  int m_responseFd;
  bool m_bRunningMtrace;


};

#endif
