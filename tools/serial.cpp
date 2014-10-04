#include <iostream> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/signal.h> 
#include <sys/ioctl.h> 
#include <sys/poll.h> 
#include <termios.h> 
#include "serial.h"
#include "log.h"

#define LOG_TAG "Serial"

namespace tools {

bool Serial::open()
{
  struct termios newtio; 
  m_fd = ::open( m_dev_name, O_RDWR | O_NOCTTY ); 
  if ( m_fd < 0 ) 
  { 
  // ȭ�� ���� ����
  LOGE( "Device OPEN FAIL %s\n", m_dev_name ); 
  return false; 
  } 
  LOGV("open %s\n", m_dev_name);
  // �ø��� ��Ʈ ȯ���� �����Ѵ�. 
  memset(&newtio, 0, sizeof(newtio)); 
  newtio.c_iflag = IGNPAR; // non-parity 
  newtio.c_oflag = 0; 
  newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts 
  switch( m_baud ) 
  { 
    case SB115200: 
      newtio.c_cflag |= B115200; break; 
    //case 57600 : newtio.c_cflag |= B57600; break; 
    case SB38400: 
      newtio.c_cflag |= B38400; break; 
    //case 19200 : newtio.c_cflag |= B19200; break; 
    //case 9600 : newtio.c_cflag |= B9600; break; 
    //case 4800 : newtio.c_cflag |= B4800; break; 
    //case 2400 : newtio.c_cflag |= B2400; break; 
    default: 
      newtio.c_cflag |= B115200; break; 
  } 
  //set input mode (non-canonical, no echo,.....) 
  newtio.c_lflag = 0; 
  newtio.c_cc[VTIME] = 10; // timeout 0.1�� ����
  newtio.c_cc[VMIN] = 1; // �ּ� n ���� ���� ������ ���
  tcflush ( m_fd, TCIFLUSH ); 
  tcsetattr( m_fd, TCSANOW, &newtio ); 

  return true; 
} 

/*
void Serial::run()
{
  struct pollfd fds;
  int ret;
  
  fds.fd = m_fd;
  fds.events = POLLIN;
  while(1){
    ret = poll(&fds, 1, -1);
    m_rn->onRead();
  }
}
*/
int Serial::close()
{
  ::close(m_fd);
}

/*
void Serial::startRead(SerialReadNoti* rn)
{
  m_rn = rn;
  m_thread = new Thread<Serial>(&Serial::run, this, "SerialThread");
}
*/
int Serial::write(const char* buf, int len)
{
  return ::write(m_fd, buf, len);
}
int Serial::read(char* buf, int len)
{
  return ::read(m_fd, buf, len);
}

};

