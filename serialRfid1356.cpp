#include <stdio.h>
#include "serialRfid1356.h"
#include "tools/log.h"
#include "tools/utils.h"

#define LOG_TAG "SerialRfid1356"

#define CMD_OPEN 
bool SerialRfid1356::open()
{
  bool ret = false;
  LOGI("open +++\n");
  static const byte cmd[] = {0x04,0x00,0x8A,0xFF};
  SerialRfid::open();
  m_serial.write(cmd, sizeof(cmd));

  LOGI("open write---\n");
  byte buf[5];
  try{
    int len = m_serial.read(buf, 5, 3000);
    ret = true;
  }
  catch(AsyncSerial::Exception e){
    LOGE("AsyncSerial::Exception %d\n", e);
  }
  LOGI("open ---\n");
  return ret;
} 

//-1:fail 0:same 1:success
int SerialRfid1356::requestData()
{
  static const byte cmd[] = {0x04,0x00,0x60,0xFF};
  static time_t shadowTime = 0;

  m_serial.write(cmd, sizeof(cmd));

  int len = m_serial.read(m_reciveBuf, RECEIVE_BUF_SIZE, 2000);
  if(len != 6)
    return -1;
  if(m_reciveBuf[0] == 0x06 && m_reciveBuf[5] == 0xFF){
    time_t t = time(NULL);
    sprintf(m_serialnumberBuf, "%03d%03d%03d%03d", m_reciveBuf[1],m_reciveBuf[2],m_reciveBuf[3],m_reciveBuf[4]);
    if((t - shadowTime < 6) && !strncmp(m_serialnumberBuf, m_serialnumberShadowBuf, SERIALNUMBER_BUF_SIZE-1)){
      LOGV("same serial number\n");
      return 0;
    }
    strcpy(m_serialnumberShadowBuf, m_serialnumberBuf);
    shadowTime = t;
    return 1;
  }
  
  return -1;
}

//===========================================================================================
// class SerialRfid1356_

#undef LOG_TAG
#define LOG_TAG "SerialRfid1356_"

#define BTSTART 0xAA
#define BTFIRSTBLOCK 0x00
#define BTNUMBERBLOCK 0x00
#define BTUIDONOFF 0x01
#define BTMODE 0x01
#define BTBUZZER 0x01
#define BTEND 0xBB
bool SerialRfid1356_::open()
{
  LOGI("open +++\n");
  SerialRfid::open();
  return true;
} 

//-1:fail 0:same 1:success
int SerialRfid1356_::requestData()
{
  static time_t shadowTime = 0;
  //printf("requestData\n");
  int len = m_serial.read(m_reciveBuf, RECEIVE_BUF_SIZE, -1);
  //printf("requestData %d\n", len);
  //tools::utils::hexdump("rece", m_reciveBuf, len);
  if(len != 5)
    return -1;
  if(m_reciveBuf[0] ^ m_reciveBuf[1] ^ m_reciveBuf[2] ^ m_reciveBuf[3] == m_reciveBuf[4]){
    time_t t = time(NULL);
    sprintf(m_serialnumberBuf, "%03d%03d%03d%03d", m_reciveBuf[3],m_reciveBuf[2],m_reciveBuf[1],m_reciveBuf[0]);
    if((t - shadowTime < 6) && !strncmp(m_serialnumberBuf, m_serialnumberShadowBuf, SERIALNUMBER_BUF_SIZE-1)){
      LOGV("same serial number\n");
      return 0;
    }
    strcpy(m_serialnumberShadowBuf, m_serialnumberBuf);
    shadowTime = t;
    return 1;
  }
  
  return -1;
}


