#ifndef _SERIAL_HEADER
#define _SERIAL_HEADER

#include <string.h>
#include "thread.h"

namespace tools {

class Serial {
public:
  enum Baud {
    SB38400,
    SB115200,
  };
/*
  class SerialReadNoti {
  public:
    virtual void onRead() = 0;
  };
*/  
  Serial(const char* path, Baud baud, int vtime = 10, int vmin = 1):m_fd(-1), m_vtime(vtime), m_vmin(vmin){
    strcpy(m_dev_name, path);
    m_baud = baud;
  }

  bool open();
  //void run();
  int close();
  //void startRead(SerialReadNoti* rn);
  int write(const char* buf, int len);
  int read(char* buf, int len);
  //int getFD() const {return m_fd;}

protected:
  int m_fd;
  
private:
  Baud m_baud;
  char m_dev_name[50];
  int m_vtime;
  int m_vmin;
  //SerialReadNoti* m_rn;
  //Thread<Serial>* m_thread;
  
};



}

#endif

