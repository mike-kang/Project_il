#ifndef _WEBSERVICE_HEADER
#define _WEBSERVICE_HEADER

#include "tools/event.h"
#include "tools/queue.h"

#include "tools/thread.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

class WebService {
public:
  static const int MAX_POLL_TIME = 3000;
  typedef  void (*CCBFunc)(void *client_data);
  struct req_data {
    int retval;
    int fd;
    int timelimit;
    CCBFunc m_cbfunc;
    void *m_client;
    Condition m_request_completed;
    Mutex mtx;
    char* m_cmd;
    int m_cmd_offset;
    req_data(char* cmd, int t):m_cmd(cmd), m_cmd_offset(0), timelimit(t) { m_cbfunc=NULL;};
    req_data(char* cmd, int cmd_offset, int t):m_cmd(cmd), m_cmd_offset(cmd_offset), timelimit(t) { m_cbfunc=NULL;};
    req_data(char* cmd, CCBFunc cbfunc, void* client):m_cmd(cmd), m_cmd_offset(0), m_cbfunc(cbfunc), m_client(client){timelimit=-1;};
    virtual ~req_data() { delete m_cmd; }
  };

  enum Except{
    EXCEPTION_CREATE_SOCKET,
    EXCEPTION_CONNECT,
    EXCEPTION_SEND_COMMAND,
  };

  enum Ret {
    RET_SUCCESS,
    RET_CREATE_SOCKET_FAIL,
    RET_CONNECT_FAIL,
    RET_SEND_CMD_FAIL,
    RET_POLL_FAIL,
    RET_POLL_TIMEOUT,
  };

  WebService(const char* ip, int port);
  ~WebService(){};
  int start();

//request
  int request_CodeDataSelect(char *sMemcoCd, char* sSiteCd, int timelimit);
  int request_GetNetInfo(int timelimit, CCBFunc cbfunc=NULL);
  int request_RfidInfoSelectAll(char *sMemcoCd, char* sSiteCd, int timelimit);
  void request_RfidInfoSelectAll(char *sMemcoCd, char* sSiteCd, CCBFunc cbfunc, void* client);
  int request_RfidInfoSelect(char *sMemcoCd, char* sSiteCd, char* serialnum, int 
  timelimit);
  void request_ServerTimeGet(CCBFunc cbfunc, void* client);
  int request_StatusUpdate(char *sGateType, char* sSiteCd, char* sDvLoc, char* sdvNo, char* sIpAddress, char* sMacAddress, int timelimit);

private:
  void run(); 
  
  void _processRequest(void* arg);

  int send_command(char *cmd);

  
  Thread<WebService> *m_thread;
  TEvent<WebService>* m_event;
  tools::Queue<TEvent< WebService> > m_requestQ;
  Condition m_request_completed;
  Mutex mtx;
  char m_serverIP[16]; //XXX.XXX.XXX.XXX
  int m_port;
  struct sockaddr_in m_remote;
};




#endif

