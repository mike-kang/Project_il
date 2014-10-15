#include <iostream>
#include <fstream>
#include "webservice.h"
#include "../tools/log.h"

using namespace std;

#define LOG_TAG "TEST_MAIN"

#define RCVHEADERBUFSIZE 1024
#define RCVBUFSIZE 4096

WebService* m_ws;
#if 0
bool GetNetInfo()
{
  int sock;
  sock = m_ws->request_GetNetInfo(3000);  //blocked I/O
  if(sock < 0){
    cout << "request_GetNetInfo fail!" << endl;
    throw 1;
  }
  char buf[RCVHEADERBUFSIZE];
  int readlen;
  memset(buf, 0xff, RCVHEADERBUFSIZE);
  readlen = recv(sock, buf, RCVHEADERBUFSIZE - 1, 0);
  //header
  if(readlen <= 0){
    cout << "request_GetNetInfo recv fail!" << endl;
    throw 2;
  }
      
  LOGV("received: %d, last+1:%d\n", readlen, buf[readlen]);
  buf[readlen] = '\0';
  //debug start
  std::ofstream oOut("received_GetNetInfo.txt");
  oOut << buf;
  oOut.close();
  //debug end
  char* p = strstr(buf, " ");
  char* e = strstr(p+1, " "); *e = '\0';
  int retVal = atoi(p+1);
  LOGV("return val: %d\n", retVal);
  if(retVal != 200){
    LOGE("fail ---\n"); 
  }
  p = strstr(e+1, "\nContent-Length:");
  //LOGV("p: %x\n", p);
  p+=17; //"\nContent-Length: " 
  e = strstr(p, "\r"); *e = '\0';
  int contentsLength = atoi(p);
  LOGV("length: %d\n", contentsLength);

  //contents
  p = e+2;
  p = strstr(p, "\n");
  p = strstr(p, "boolean");
  p = strstr(p, ">");
  
  //LOGV("test:%s\n", p+1);
  //cout << buf << endl;
  close(sock);

  return (*(p+1)=='t');
}

void CodeDataSelect()
{
  int sock;
  sock = m_ws->request_CodeDataSelect("MC00000003", "ST00000005", 9000);  //blocked I/O
  if(sock < 0){
    cout << "request_GetNetInfo fail!" << endl;
    return;
  }
  char headerbuf[RCVHEADERBUFSIZE];
  int readlen;

  //header
  readlen = recv(sock, headerbuf, RCVHEADERBUFSIZE - 1, 0);
  if(readlen <= 0){
    cout << "request_GetNetInfo recv fail!" << endl;
    return;
  }
    
  LOGV("received: %d, last+1:%d\n", readlen, headerbuf[readlen]);
  //debug start
  std::ofstream oOut("received_CodeDataSelect.txt");
  oOut << headerbuf;
  //debug end
  char* p = strstr(headerbuf, " ");
  char* e = strstr(p+1, " "); *e = '\0';
  int retVal = atoi(p+1);
  LOGV("return val: %d\n", retVal);
  if(retVal != 200){
    LOGE("fail ---\n"); 
    return;
  }
  p = strstr(e+1, "\nContent-Length:");
  //LOGV("p: %x\n", p);
  p+=17; //"\nContent-Length: " 
  e = strstr(p, "\r"); *e = '\0';
  int contentsLength = atoi(p);
  LOGV("length: %d\n", contentsLength);

  //contents
  char* buf = new char[contentsLength+1];
  int offset = readlen - (e+4 - headerbuf);
  
  memcpy(buf, e+4, offset);
  int nleaved = contentsLength - offset;
  while(nleaved){
    readlen = recv(sock, buf + offset, contentsLength, 0);
    //debug start
    oOut << buf + offset;
    //debug end
    nleaved -= readlen;
    offset += readlen;
    LOGV("read:%d, offset:%d, nleaved:%d\n", readlen, offset, nleaved);
  }
        
  buf[contentsLength] = '\0';
  oOut.close();
  //cout << "contents=" << buf << endl;
  delete buf;
  close(sock);

}

void cbServerTimeGet(void* arg)
{

}
/*
bool ServerTimeGet()
{
  int sock;
  sock = m_ws->request_ServerTimeGet(cbServerTimeGet, NULL);  //blocked I/O
  if(sock < 0){
    cout << "request_ServerTimeGet fail!" << endl;
    throw 1;
  }
  char buf[RCVHEADERBUFSIZE];
  int readlen;
  memset(buf, 0xff, RCVHEADERBUFSIZE);
  readlen = recv(sock, buf, RCVHEADERBUFSIZE - 1, 0);
  //header
  if(readlen <= 0){
    cout << "request_ServerTimeGet recv fail!" << endl;
    throw 2;
  }
      
  LOGV("received: %d, last+1:%d\n", readlen, buf[readlen]);
  buf[readlen] = '\0';
  //debug start
  std::ofstream oOut("received_ServerTimeGet.txt");
  oOut << buf;
  oOut.close();
  //debug end
  char* p = strstr(buf, " ");
  char* e = strstr(p+1, " "); *e = '\0';
  int retVal = atoi(p+1);
  LOGV("return val: %d\n", retVal);
  if(retVal != 200){
    LOGE("fail ---\n"); 
  }
  p = strstr(e+1, "\nContent-Length:");
  //LOGV("p: %x\n", p);
  p+=17; //"\nContent-Length: " 
  e = strstr(p, "\r"); *e = '\0';
  int contentsLength = atoi(p);
  LOGV("length: %d\n", contentsLength);

  //contents
  p = e+2;
  p = strstr(p, "\n");
  p = strstr(p, "dateTime");
  p = strstr(p, ">");
  
  //LOGV("test:%s\n", p+1);
  //cout << buf << endl;
  close(sock);

  return (*(p+1)=='t');
}
*/

bool StatusUpdate()
{
  int sock;
  sock = m_ws->request_StatusUpdate("IN", "ST00000005", "0001", "1", "192.168.190.130", "00-0c-29-95-30-24", 3000);  //blocked I/O
  if(sock < 0){
    cout << "request_StatusUpdate fail!" << endl;
    throw 1;
  }
  char buf[RCVHEADERBUFSIZE];
  int readlen;
  memset(buf, 0xff, RCVHEADERBUFSIZE);
  readlen = recv(sock, buf, RCVHEADERBUFSIZE - 1, 0);
  //header
  if(readlen <= 0){
    cout << "request_StatusUpdate recv fail!" << endl;
    throw 2;
  }
      
  LOGV("received: %d, last+1:%d\n", readlen, buf[readlen]);
  buf[readlen] = '\0';
  //debug start
  std::ofstream oOut("received_StatusUpdate.txt");
  oOut << buf;
  oOut.close();
  //debug end
  char* p = strstr(buf, " ");
  char* e = strstr(p+1, " "); *e = '\0';
  int retVal = atoi(p+1);
  LOGV("return val: %d\n", retVal);
  if(retVal != 200){
    LOGE("fail ---\n"); 
    throw 3;
  }
  p = strstr(e+1, "\nContent-Length:");
  //LOGV("p: %x\n", p);
  p+=17; //"\nContent-Length: " 
  e = strstr(p, "\r"); *e = '\0';
  int contentsLength = atoi(p);
  LOGV("length: %d\n", contentsLength);

  //contents
  p = e+2;
  p = strstr(p, "\n");
  p = strstr(p, "boolean");
  p = strstr(p, ">");
  
  //LOGV("test:%s\n", p+1);
  //cout << buf << endl;
  close(sock);

  return (*(p+1)=='t');
}
#endif

#define DUMP_CASE(x) case x: return #x;

char* dump_error(WebService::Except e)
{
  switch(e){
    DUMP_CASE (WebService::EXCEPTION_CREATE_SOCKET)
    DUMP_CASE (WebService::EXCEPTION_CONNECT)
    DUMP_CASE (WebService::EXCEPTION_SEND_COMMAND)
    DUMP_CASE (WebService::EXCEPTION_POLL_FAIL)
    DUMP_CASE (WebService::EXCEPTION_POLL_TIMEOUT)
    DUMP_CASE (WebService::EXCEPTION_PARSING_FAIL)
  }

}

void cbGetNetInfo(void *client_data, int status, void* ret)
{
  LOGV("cbGetNetInfo status:%d, ret:%d\n", status, *((bool*)ret));
}

void cbCodeDataSelect(void *client_data, int status, void* ret)
{
  char* xml_buf = (char*)ret;
  LOGV("cbCodeDataSelect status:%d, ret:%s\n", status, xml_buf);
  cout << "***cbCodeDataSelect: " << xml_buf << endl;
  delete xml_buf;
}

void cbRfidInfoSelectAll(void *client_data, int status, void* ret)
{
  LOGV("cbRfidInfoSelectAll status:%d\n", status);
}

void cbRfidInfoSelect(void *client_data, int status, void* ret)
{
  char* xml_buf = (char*)ret;
  LOGV("cbRfidInfoSelect status:%d, ret:%s\n", status, xml_buf);
  cout << "***cbRfidInfoSelect: " << xml_buf << endl;
  delete xml_buf;
}

void cbServerTimeGet(void *client_data, int status, void* ret)
{
  char* time_buf = (char*)ret;
  LOGV("cbServerTimeGet status:%d, ret:%s\n", status, time_buf);
  delete time_buf;
}

void cbStatusUpdate(void *client_data, int status, void* ret)
{
  LOGV("cbStatusUpdate status:%d, ret:%d\n", status, *((bool*)ret));
}

int main()
{
  log_init(TYPE_CONSOLE, NULL);


  m_ws = new WebService("192.168.0.7", 8080);
  //m_ws->start();

  bool ret;
  char* xml_buf;
  char* time_buf;
 
  try{
    //xml_buf = m_ws->request_CodeDataSelect("MC00000003", "ST00000005", "0001", 3000);  //blocked I/O
    xml_buf = m_ws->request_CodeDataSelect("MC00000003", "ST00000005", "0001", cbCodeDataSelect, NULL);  //blocked I/O
    if(xml_buf){
      cout << "***CodeDataSelect: " << xml_buf << endl;
      delete xml_buf;
    }
  }
  catch(WebService::Except e){
    LOGE("request_CodeDataSelect: %s\n", dump_error(e));
  }

  try{
    //ret = m_ws->request_GetNetInfo(3000);  //blocked I/O
    ret = m_ws->request_GetNetInfo(cbGetNetInfo, NULL);  //blocked I/O
    LOGV("***GetNetInfo: %d\n", ret);
  }
  catch(WebService::Except e){
    LOGE("request_GetNetInfo: %s\n", dump_error(e));
  }

  try{
    m_ws->request_RfidInfoSelectAll("MC00000003", "ST00000005", 7000, "employee.xml");  //blocked I/O
    //m_ws->request_RfidInfoSelectAll("MC00000003", "ST00000005", cbRfidInfoSelectAll, NULL, "employee.xml");  //blocked I/O
  }
  catch(WebService::Except e){
    LOGE("request_RfidInfoSelectAll: %s\n", dump_error(e));
  }

  try{
    xml_buf = m_ws->request_RfidInfoSelect("MC00000003", "ST00000005", "253153215009", 3000);  //blocked I/O
    //xml_buf = m_ws->request_RfidInfoSelect("MC00000003", "ST00000005", "253153215009", cbRfidInfoSelect, NULL);  //blocked I/O
    if(xml_buf){
      cout << "***RfidInfoSelect: " << xml_buf << endl;
      delete xml_buf;
    }
  }
  catch(WebService::Except e){
    LOGE("request_RfidInfoSelect: %s\n", dump_error(e));
  }
/*
  try{
    time_buf = m_ws->request_ServerTimeGet(3000);  //blocked I/O
    //time_buf = m_ws->request_ServerTimeGet(cbServerTimeGet, NULL);  //blocked I/O
    if(time_buf){
      cout << "***ServerTimeGet: " << time_buf << endl;
      delete time_buf;
    }
  }
  catch(WebService::Except e){
    LOGE("request_ServerTimeGet: %s\n", dump_error(e));
  }

  try{
    ret = m_ws->request_StatusUpdate("IN", "ST00000005", "0001", "1", "192.168.190.130", "00-0c-29-95-30-24", 3000);  //blocked I/O
    //ret = m_ws->request_StatusUpdate("IN", "ST00000005", "0001", "1", "192.168.190.130", "00-0c-29-95-30-24", cbStatusUpdate, NULL);  //blocked I/O
    LOGV("***StatusUpdate: %d\n", ret);
  }
  catch(WebService::Except e){
    LOGE("request_StatusUpdate: %s\n", dump_error(e));
  }
*/

  
  while(1)
    sleep(1);

  return 0;
}

