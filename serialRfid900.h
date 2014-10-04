#ifndef _SERIALR_RFID900_HEADER
#define _SERIALR_RFID900_HEADER

#include "serialRfid.h"

#define RECEIVE_BUF_SIZE 100

class SerialRfid900 : public SerialRfid{
public:
  SerialRfid900(const char* path):SerialRfid(path, Serial::SB115200)
  {
    m_reciveBuf = new char[RECEIVE_BUF_SIZE];
    m_serialnumberBuf = new char[SERIALNUMBER_BUF_SIZE];
    m_serialnumberShadowBuf = new char[SERIALNUMBER_BUF_SIZE];
    m_serialnumberShadowBuf[0] = '\0';
  }
  virtual bool open();
  virtual bool requestData();

private:
  char* m_serialnumberShadowBuf;
  
};




#endif

