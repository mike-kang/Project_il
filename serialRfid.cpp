#include "serialRfid.h"
#include "tools/log.h"

#define LOG_TAG "SerialRfid"

bool SerialRfid::open()
{
  bool ret;
  LOGI("open +++\n");
  ret = m_serial.open();
  LOGI("open ---\n");
}

int SerialRfid::close()
{
  m_serial.close();
}

void SerialRfid::start(int interval, SerialRfidDataNoti* dn)
{
  m_running = true;
  m_interval = interval;
  m_dn = dn;
  m_thread = new Thread<SerialRfid>(&SerialRfid::run, this, "SerialRfidThread");
}

void SerialRfid::stop()
{
  m_running = false;
  delete m_thread;
  m_thread = NULL;
}
/*
void SerialRfid::pause()
{
  
}
void SerialRfid::resume()
{

}
*/
/*
void SerialRfid::registerDataNoti(SerialRfidDataNoti* dn)
{
  m_dn = dn;
}
*/

void SerialRfid::run()
{
  int interval = m_interval * 1000;
  bool bNeedInterval = true;
  
  while(m_running){
    if(bNeedInterval) 
      usleep(interval);
    if(!requestData()){
      //LOGE("requestData fail\n");
      bNeedInterval = true;
      continue;
    }
    m_dn->onData(m_serialnumberBuf);
    bNeedInterval = false;
  }

}


