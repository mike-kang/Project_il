#include <stdio.h>
#include "webservice.h"
#include <sys/poll.h> 
#include "tools/log.h"

#define LOG_TAG "WebService"

WebService::WebService(const char* ip, int port)
{
  strcpy(m_serverIP, ip);
  m_port = port;

}

int WebService::start()
{
  m_remote.sin_family = AF_INET;
  inet_pton(AF_INET, m_serverIP, (void *)(&(m_remote.sin_addr.s_addr)));
  m_remote.sin_port = htons(m_port);
  m_thread = new Thread<WebService>(&WebService::run, this, "WebServiceThread");
}

void WebService::run()
{
  while(true)
  {
    //dispatch event
    m_event = m_requestQ.pop();
    
    //LOGI("WebService::run: %p\n", m_event);
    if(m_event){
      (this->*(m_event->ev_processFunc))(m_event->ev_data);
      delete m_event;
    }
    else{
      LOGI("Terminate WebService Request thread\n");
      break;
    }
  }

}

#if 0
int WebService::request_CodeDataSelect(char *sMemcoCd, char* sSiteCd, int timelimit)
{
  int fd = -1;
  LOGV("request_CodeDataSelect\n");
  char *cmd = new char[300];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/CodeDataSelect?sMemcoCd=%s&sSiteCd=%s&sType=T&sGroupCd=0007 HTTP/1.1\r\nHost: %s\r\n\r\n"
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
#else
//SOAP 1.1
#define SOAP_2
#ifdef SOAP_1_1
#define SOAP_HEADER_SZ 161 //except ip & length
#elif defined SOAP_2
#define SOAP_HEADER_SZ 112 //except ip & length
#endif

int WebService::request_CodeDataSelect(char *sMemcoCd, char* sSiteCd, int timelimit)
{
  int fd = -1;
  LOGV("request_CodeDataSelect SOAP 1.1\n");
  char *cmd = new char[1024];

#ifdef SOAP_1_1
  sprintf(cmd+400, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
    "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope\">\r\n"
    "  <soap:Body>\r\n"
    "    <CodeDataSelect xmlns=\"http://tempuri.org/\">\r\n"
    "      <sMemcoCd>%s</sMemcoCd>\r\n"
    "      <sSiteCd>%s</sSiteCd>\r\n"
    "      <sType>T</sType>\r\n"
    "      <sGroupCd>0007</sGroupCd>\r\n"
    "    </CodeDataSelect>\r\n"
    "  </soap:Body>\r\n"
    "</soap:Envelope>",  sMemcoCd, sSiteCd);
#elif defined SOAP_2
  sprintf(cmd+400, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
    "<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\r\n"
    "  <soap12:Body>\r\n"
    "    <CodeDataSelect xmlns=\"http://tempuri.org/\">\r\n"
    "      <sMemcoCd>%s</sMemcoCd>\r\n"
    "      <sSiteCd>%s</sSiteCd>\r\n"
    "      <sType>T</sType>\r\n"
    "      <sGroupCd>0007</sGroupCd>\r\n"
    "    </CodeDataSelect>\r\n"
    "  </soap12:Body>\r\n"
    "</soap12:Envelope>",  sMemcoCd, sSiteCd);
#endif
  int len1 = strlen(cmd + 400);

  int headerlength = SOAP_HEADER_SZ + strlen(m_serverIP) + 3; // strlen(itoa(len1,10)) = 3
  int offset = 400 - headerlength;
 
#ifdef SOAP_1_1
  sprintf(cmd + offset, "POST /WebService/ItlogService.asmx HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: text/xml; charset=utf-8\r\n"
    "Content-Length: %d\r\n"
    "SOAPAction: \"http://tempuri.org/CodeDataSelect\"\r\n\r\n", m_serverIP, len1);
#elif defined SOAP_2   
  sprintf(cmd + offset, "POST /WebService/ItlogService.asmx HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: text/xml; charset=utf-8\r\n"
    "Content-Length: %d\r\n\r\n", m_serverIP, len1);
#endif
  int len2 = strlen(cmd + offset);

  cmd[400] = '<';
  LOGV(cmd + offset);    

  req_data* rd = new req_data(cmd, offset, timelimit);

  rd->mtx.lock();
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

  rd->m_request_completed.wait(rd->mtx);
  rd->mtx.unlock();

  if(rd->retval == RET_SUCCESS)
    fd = rd->fd;

  delete rd;

  LOGV("strlen=%d %d\n", len1, len2);
  return fd;
}

#endif
int WebService::request_GetNetInfo(int timelimit, CCBFunc cbfunc)
{
  int fd = -1;
  LOGV("request_GetNetInfo\n");
  char *cmd = new char[100];
  sprintf(cmd,"GET /WebService/ItlogService.asmx/GetNetInfo? HTTP/1.1\r\nHost: %s\r\n\r\n", m_serverIP);
  req_data* rd = new req_data(cmd, timelimit);

  if(!cbfunc)
    rd->mtx.lock();
  
  TEvent<WebService>* e = new TEvent<WebService>(&WebService::_processRequest, rd);
  m_requestQ.push(e);

  if(!cbfunc){
    rd->m_request_completed.wait(rd->mtx);
    rd->mtx.unlock();
  }
  if(rd->retval == RET_SUCCESS)
    fd = rd->fd;

  delete rd;
  return fd;
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



