#include <iostream>
#include <fstream>
#include "webservice.h"
#include <sys/poll.h> 
#include "tools/log.h"

#define LOG_TAG "WebService"

#define HTTP_OK 200
#define RCVHEADERBUFSIZE 1024

WebService::WebService(const char* ip, int port)
{
  strcpy(m_serverIP, ip);
  m_port = port;
  m_remote.sin_family = AF_INET;
  inet_pton(AF_INET, m_serverIP, (void *)(&(m_remote.sin_addr.s_addr)));
  m_remote.sin_port = htons(m_port);
}

/*
int WebService::start()
{
  m_remote.sin_family = AF_INET;
  inet_pton(AF_INET, m_serverIP, (void *)(&(m_remote.sin_addr.s_addr)));
  m_remote.sin_port = htons(m_port);
  m_thread = new Thread<WebService>(&WebService::run, this, "WebServiceThread");
}
*/
bool WebService::WebApi::parsingHeader(char* buf, char **startContent, int* contentLength, int* readByteContent)
{
  int readlen = recv(m_sock, buf, RCVHEADERBUFSIZE - 1, 0);
  //header
  if(readlen <= 0){
    LOGE("Parsing fail: receive=<0\n"); 
    return false;
  }
      
  LOGV("received: %d\n", readlen);

#ifdef DEBUG
  buf[readlen] = '\0';
  oOut << buf;
#endif  
  char* p = strstr(buf, " ");  // buf is "HTTP/1.1 200 OK ..."
  char* e = strstr(p+1, " "); *e = '\0';
  int retVal = atoi(p+1);
  //LOGV("return val: %d\n", retVal);
  if(retVal != HTTP_OK){
    LOGE("not HTTP_OK %d\n", retVal); 
    return false;
  }
  p = strstr(e+1, "\nContent-Length:");
  //LOGV("p: %x\n", p);
  p+=17; //"\nContent-Length: " 
  e = strstr(p, "\r"); *e = '\0';
  *contentLength = atoi(p);
  LOGV("length: %d\n", *contentLength);
  *startContent = e + 4;  // \r\n\r\n
  *readByteContent = readlen - (*startContent - buf);
  LOGV("parsingHeader: contentLength=%d, readByteContent=%d\n", *contentLength, *readByteContent);
  return true;
}

bool WebService::GetNetInfo_WebApi::parsing()
{
  char headerbuf[RCVHEADERBUFSIZE];
  char* startContent;
  int contentLength;
  int readByteContent;

  if(!parsingHeader(headerbuf, &startContent, &contentLength, &readByteContent))
    return false;

  //contents
  char* p;
  p = strstr(startContent, "\n");
  p = strstr(p, "boolean");
  p = strstr(p, ">");
  
  m_ret = (*(p+1)=='t');
  return true;
}



bool WebService::CodeDataSelect_WebApi::parsing()
{
  char headerbuf[RCVHEADERBUFSIZE];
  char* startContent;
  int contentLength;
  int readByteContent;

  if(!parsingHeader(headerbuf, &startContent, &contentLength, &readByteContent))
    return false;

  //contents
  char* buf = new char[contentLength+1];
  
  memcpy(buf, startContent, readByteContent);
  int nleaved = contentLength - readByteContent;
  while(nleaved){
    int readlen = recv(m_sock, buf + readByteContent, nleaved, 0);
#ifdef DEBUG
    buf[readByteContent + readlen] = '\0';
    oOut << buf + readByteContent;
#endif
    nleaved -= readlen;
    readByteContent += readlen;
    LOGV("read:%d, readByteContent:%d, nleaved:%d\n", readlen, readByteContent, nleaved);
  }
        
  buf[contentLength] = '\0';
  m_pRet = buf;

  return true;
}

//#define SOAP_2_CODEDATASELECT //not work
#define SOAP_HEADER_SZ 112 //except ip & length

char* WebService::request_CodeDataSelect(char *sMemcoCd, char* sSiteCd, char* sDvLoc, int timelimit, CCBFunc cbfunc, void* client)
{
  char* ret = NULL;
  char *cmd;
  int cmd_offset = 0;
#ifdef SOAP_2_CODEDATASELECT
  LOGV("request_CodeDataSelect SOAP 2\n");
  cmd = new char[1024];
  sprintf(cmd+400, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
    "<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\r\n"
    "  <soap12:Body>\r\n"
    "    <CodeDataSelect xmlns=\"http://tempuri.org/\">\r\n"
    "      <sMemcoCd>%s</sMemcoCd>\r\n"
    "      <sSiteCd>%s</sSiteCd>\r\n"
    "      <sType>T</sType>\r\n"
    "      <sGroupCd>0007'+AND+CODE='%s</sGroupCd>\r\n"
    "    </CodeDataSelect>\r\n"
    "  </soap12:Body>\r\n"
    "</soap12:Envelope>",  sMemcoCd, sSiteCd, sDvLoc);
  int len1 = strlen(cmd + 400);

  int headerlength = SOAP_HEADER_SZ + strlen(m_serverIP) + 3; // strlen(itoa(len1,10)) = 3
  cmd_offset = 400 - headerlength;
 

  sprintf(cmd + cmd_offset, "POST /WebService/ItlogService.asmx HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: text/xml; charset=utf-8\r\n"
    "Content-Length: %d\r\n\r\n", m_serverIP, len1);

  //int len2 = strlen(cmd + cmd_offset);
  cmd[400] = '<';

#else
  LOGV("request_CodeDataSelect\n");
  cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/CodeDataSelect?sMemcoCd=%s&sSiteCd=%s&sType=T&sGroupCd=0007'+AND+CODE='%s HTTP/1.1\r\nHost: %s\r\n\r\n"
    , sMemcoCd, sSiteCd, sDvLoc, m_serverIP);
#endif

  //LOGV("cmd:%s\n", cmd+cmd_offset);
  CodeDataSelect_WebApi* wa;

  if(cbfunc){
    wa = new CodeDataSelect_WebApi(this, cmd, cmd_offset, cbfunc, client);
    wa->processCmd();
  }
  else{
    wa = new CodeDataSelect_WebApi(this, cmd, cmd_offset, timelimit);
  
    int status = wa->processCmd();

    switch(status){
      case RET_CREATE_SOCKET_FAIL:
        throw EXCEPTION_CREATE_SOCKET;
      case RET_CONNECT_FAIL:
        throw EXCEPTION_CONNECT;
      case RET_SEND_CMD_FAIL:
        throw EXCEPTION_SEND_COMMAND;
      case RET_POLL_FAIL:
        throw EXCEPTION_POLL_FAIL;
      case RET_POLL_TIMEOUT:
        throw EXCEPTION_POLL_TIMEOUT;
      case RET_PARSING_FAIL:
        throw EXCEPTION_PARSING_FAIL;
    }
    
    ret = (char*)wa->m_pRet;
    delete wa;
  }
  return ret;
}

bool WebService::request_GetNetInfo(int timelimit, CCBFunc cbfunc, void* client)
{
  bool ret = false;
  LOGV("request_GetNetInfo\n");
  char *cmd = new char[100];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/GetNetInfo? HTTP/1.1\r\nHost: %s\r\n\r\n", m_serverIP);

  GetNetInfo_WebApi* wa;

  if(cbfunc){
    wa = new GetNetInfo_WebApi(this, cmd, 0, cbfunc, client);
    wa->processCmd();
  }
  else{
    wa = new GetNetInfo_WebApi(this, cmd, 0, timelimit);
  
    int status = wa->processCmd();

    switch(status){
      case RET_CREATE_SOCKET_FAIL:
        throw EXCEPTION_CREATE_SOCKET;
      case RET_CONNECT_FAIL:
        throw EXCEPTION_CONNECT;
      case RET_SEND_CMD_FAIL:
        throw EXCEPTION_SEND_COMMAND;
      case RET_POLL_FAIL:
        throw EXCEPTION_POLL_FAIL;
      case RET_POLL_TIMEOUT:
        throw EXCEPTION_POLL_TIMEOUT;
      case RET_PARSING_FAIL:
        throw EXCEPTION_PARSING_FAIL;
    }
    
    ret = wa->m_ret;
    delete wa;
  }
  return ret;
}

//sync
int WebService::request_RfidInfoSelectAll(char *sMemcoCd, char* sSiteCd, int timelimit)
{
  int fd = -1;
  LOGV("request_RfidInfoSelectAll\n");
  char *cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/RfidInfoSelect?sMemcoCd=%s&sSiteCd=%s&sUtype=&sMode=A&sSearchValue= HTTP/1.1\r\nHost: %s\r\n\r\n"
    , sMemcoCd, sSiteCd, m_serverIP);
  req_data* rd = new req_data(cmd, timelimit);

  rd->mtx.lock();
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

  rd->m_request_completed.wait(rd->mtx);
  rd->mtx.unlock();

  if(rd->retval == RET_SUCCESS)
    fd = rd->fd;

  delete rd;
  return fd;
}

//async
void WebService::request_RfidInfoSelectAll(char *sMemcoCd, char* sSiteCd, CCBFunc cbfunc, void* client)
{
  LOGV("request_RfidInfoSelectAll\n");
  char *cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/RfidInfoSelect?sMemcoCd=%s&sSiteCd=%s&sUtype=&sMode=A&sSearchValue= HTTP/1.1\r\nHost: %s\r\n\r\n"
    , sMemcoCd, sSiteCd, m_serverIP);
  req_data* rd = new req_data(cmd, cbfunc, client);
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

}


//sync
int WebService::request_RfidInfoSelect(char *sMemcoCd, char* sSiteCd, char* serialnum, int timelimit)
{
  int fd = -1;
  LOGV("request_RfidInfoSelect\n");
  char *cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/RfidInfoSelect?sMemcoCd=%s&sSiteCd=%s&sUtype=&sMode=R&sSearchValue=RFID_CAR='%s' HTTP/1.1\r\nHost: %s\r\n\r\n"
    , sMemcoCd, sSiteCd, serialnum, m_serverIP);

  LOGV("cmd:%s\n", cmd);
  req_data* rd = new req_data(cmd, timelimit);
  rd->mtx.lock();
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

  rd->m_request_completed.wait(mtx);
  rd->mtx.unlock();

  if(rd->retval == RET_SUCCESS)
    fd = rd->fd;

  delete rd;
  return fd;
}

//async
void WebService::request_ServerTimeGet(WebService::CCBFunc cbfunc, void* client)
{
  LOGV("request_ServerTimeGet\n");
  char *cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/ServerTimeGet? HTTP/1.1\r\nHost: %s\r\n\r\n", m_serverIP);
  req_data* rd = new req_data(cmd, cbfunc, client);
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);
}

int WebService::request_StatusUpdate(char *sGateType, char* sSiteCd, char* sDvLoc, char* sdvNo, char* sIpAddress, char* sMacAddress, int timelimit)
{
  int fd = -1;
  LOGV("request_StatusUpdate\n");
  char *cmd = new char[400];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/Status_Update?sMemcoCd=%s&sSiteCd=%s&sGateCode=%s&sGateNo=%s&sGateIp=%s&sGateMac=%s HTTP/1.1\r\nHost: %s\r\n\r\n"
    , sGateType, sSiteCd, sDvLoc, sdvNo, sIpAddress, sMacAddress, m_serverIP);

  LOGV("cmd:%s\n", cmd);
  req_data* rd = new req_data(cmd, timelimit);
  rd->mtx.lock();
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

  rd->m_request_completed.wait(mtx);
  rd->mtx.unlock();

  if(rd->retval == RET_SUCCESS)
    fd = rd->fd;

  delete rd;
  return fd;
}


void WebService::_processRequest(void* arg)
{
/*
  int ret;
  req_data* rd = (req_data*)arg;
  int sock;

  try{
    sock = send_command(rd->m_cmd + rd->m_cmd_offset);
  }
  catch (Except& e) {
    switch(e){
      case EXCEPTION_CREATE_SOCKET:
        rd->retval = RET_CREATE_SOCKET_FAIL;
        break;
      case EXCEPTION_CONNECT:
        rd->retval = RET_CONNECT_FAIL;
        break;
      case EXCEPTION_SEND_COMMAND:
        rd->retval = RET_SEND_CMD_FAIL;
        break;
    }
    goto error;
    return;
  }
  
  struct pollfd fds;
  
  fds.fd = sock;
  fds.events = POLLIN;
  ret = poll(&fds, 1, rd->timelimit);
  if(ret == -1){
    LOGE("RET_POLL_FAIL\n");
    rd->retval = RET_POLL_FAIL;
  }
  else if(ret == 0){
    LOGE("RET_POLL_TIMEOUT\n");
    rd->retval = RET_POLL_TIMEOUT;
  }
  else{
    rd->retval = RET_SUCCESS;
    rd->fd = sock;
  }

error:  
  if(!rd->m_cbfunc)
    rd->m_request_completed.notify_one();
  else
    rd->m_cbfunc(rd); 
    */
}

int WebService::send_command(char* cmd)
{
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    throw EXCEPTION_CREATE_SOCKET;
  }
  LOGV("connect\n");
  if(connect(sock, (struct sockaddr *)&m_remote, sizeof(struct sockaddr)) < 0){
    LOGE("EXCEPTION_CONNECT\n");
    throw EXCEPTION_CONNECT;
  }

  LOGV("send command\n");
  int len = send(sock, cmd, strlen(cmd), 0);
  if(len == -1){
    LOGE("EXCEPTION_SEND_COMMAND\n");
    throw EXCEPTION_SEND_COMMAND;
  }

  return sock;
}

void WebService::WebApi::run()
{
  int len;
  int ret;
  
  if((m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    LOGE("RET_CREATE_SOCKET_FAIL\n");
    m_status = RET_CREATE_SOCKET_FAIL;
    goto error;
  }

  LOGV("connect\n");
  if(connect(m_sock, (struct sockaddr *)&m_ws->m_remote, sizeof(struct sockaddr)) < 0){
    LOGE("RET_CONNECT_FAIL\n");
    m_status = RET_CONNECT_FAIL;
    goto error;
  }

  LOGV("send command\n");
  len = send(m_sock, m_cmd, strlen(m_cmd), 0);
  if(len == -1){
    LOGE("RET_SEND_CMD_FAIL\n");
    m_status = RET_SEND_CMD_FAIL;
    goto error;
  }

  //poll
  struct pollfd fds;
  
  fds.fd = m_sock;
  fds.events = POLLIN;
  ret = poll(&fds, 1, timelimit);
  if(ret == -1){
    LOGE("RET_POLL_FAIL\n");
    goto error;
  }
  else if(ret == 0){
    LOGE("RET_POLL_TIMEOUT\n");
    goto error;
  }

  //receive & parsing
  if(!parsing()){
    m_status = RET_PARSING_FAIL;
      goto error;
  }
  m_status = RET_SUCCESS;

error:
  if(m_cbfunc){
    m_cbfunc(m_client, m_status, m_pRet); 
    delete this;
  }
}

int WebService::WebApi::processCmd()
{
  m_thread = new Thread<WebApi>(&WebService::WebApi::run, this, "WebApi");
  
  if(!m_cbfunc)
    m_thread->join();
  else
    m_thread->detach();
  
  return m_status;
}

