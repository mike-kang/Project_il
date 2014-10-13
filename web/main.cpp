#include <iostream>
#include <fstream>
#include "webservice.h"
#include "../tools/log.h"

using namespace std;

#define LOG_TAG "TEST_MAIN"

#define RCVHEADERBUFSIZE 1024
#define RCVBUFSIZE 4096

WebService* m_ws;

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

int main()
{
  log_init(TYPE_CONSOLE, NULL);


  m_ws = new WebService("192.168.0.7", 8080);
  m_ws->start();

  bool ret;
  ret = GetNetInfo();
  LOGV("***GetNetInfo: %d\n", ret);
  sleep(3);
  CodeDataSelect();
  
  ret = StatusUpdate();
  LOGV("***StatusUpdate: %d\n", ret);




  
  while(1)
    sleep(1);

  return 0;
}

