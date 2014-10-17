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
#include <fstream>

class WebService {
public:
  static const int MAX_POLL_TIME = 3000;
  typedef  void (*CCBFunc)(void *client_data, int status, void* ret);

  class WebApi {
  public:
    enum Exception{
      Exception_Parsing,


    };
    int processCmd();
    int getStatus() const { return m_status; }
  protected:
    int m_status;
    int m_sock;
    int timelimit;
    CCBFunc m_cbfunc;
    void *m_client;
    Condition m_request_completed;
    Mutex mtx;
    char* m_cmd;
    int m_cmd_offset;
    void* m_pRet;
#ifdef DEBUG
    ofstream oOut; //for debug
#endif    
    virtual bool parsing() = 0;
    
    WebApi(WebService* ws, char* cmd, int cmd_offset, int t):m_ws(ws), m_cmd(cmd), m_cmd_offset(cmd_offset), timelimit(t) { m_cbfunc=NULL;} //sync
    WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):m_ws(ws), m_cmd(cmd), m_cmd_offset(cmd_offset), m_cbfunc(cbfunc), m_client(client){timelimit=-1;};  //async
    virtual ~WebApi()
    {
      close(m_sock);
      delete m_thread;
      delete m_cmd;
    }
    bool parsingHeader(char* buf, char **startContent, int* contentLength, int* readByteContent);


  private:
    void run();

    WebService* m_ws;
    Thread<WebApi>* m_thread;
  };
  
  class CodeDataSelect_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    CodeDataSelect_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t) //sync
    {
#ifdef DEBUG
      oOut.open("received_CodeDataSelect.txt");
#endif
    }
    CodeDataSelect_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
#ifdef DEBUG
      oOut.open("received_CodeDataSelect.txt");
#endif
    }
    virtual ~CodeDataSelect_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }

  };
  class GetNetInfo_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    GetNetInfo_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_GetNetInfo.txt");
#endif
    }
    GetNetInfo_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_GetNetInfo.txt");
#endif
    }
    virtual ~GetNetInfo_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }

  private:
    bool m_ret;

  };

  class RfidInfoSelectAll_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    RfidInfoSelectAll_WebApi(WebService* ws, char* cmd, int cmd_offset, int t, 
    const char* outFilename):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
#ifdef DEBUG
      oOut.open("received_RfidInfoSelectAll.txt");
#endif
      strcpy(m_filename, outFilename);
    }
    RfidInfoSelectAll_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client, 
    const char* outFilename):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
#ifdef DEBUG
      oOut.open("received_RfidInfoSelectAll.txt");
#endif
      strcpy(m_filename, outFilename);
    }

    virtual ~RfidInfoSelectAll_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }

  private:
    char m_filename[255];
  };

  class RfidInfoSelect_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    RfidInfoSelect_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
#ifdef DEBUG
      oOut.open("received_RfidInfoSelect.txt");
#endif
    }
    RfidInfoSelect_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
#ifdef DEBUG
      oOut.open("received_RfidInfoSelect.txt");
#endif
    }
    virtual ~RfidInfoSelect_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }
  };

  class ServerTimeGet_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    ServerTimeGet_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
#ifdef DEBUG
      oOut.open("received_ServerTimeGet.txt");
#endif
    }
    ServerTimeGet_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
#ifdef DEBUG
      oOut.open("received_ServerTimeGet.txt");
#endif
    }
    virtual ~ServerTimeGet_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }
  };
  
  class StatusUpdate_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    StatusUpdate_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_StatusUpdate.txt");
#endif
    }
    StatusUpdate_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_StatusUpdate.txt");
#endif
    }
    virtual ~StatusUpdate_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }
    private:
      bool m_ret;
  };

  class TimeSheetInsertString_WebApi : public WebApi {
  friend class WebService;
  public:
    virtual bool parsing();
    
    TimeSheetInsertString_WebApi(WebService* ws, char* cmd, int cmd_offset, int t):WebApi(ws, cmd, cmd_offset, t)  //sync
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_TimeSheetInsertString.txt");
#endif
    }
    TimeSheetInsertString_WebApi(WebService* ws, char* cmd, int cmd_offset, CCBFunc cbfunc, void* client):WebApi(ws, cmd, cmd_offset, cbfunc, client) //async
    {
      m_pRet = &m_ret;
#ifdef DEBUG
      oOut.open("received_TimeSheetInsertString.txt");
#endif
    }
    virtual ~TimeSheetInsertString_WebApi()
    {
#ifdef DEBUG
      oOut.close();
#endif  
    }
    private:
      bool m_ret;
  };
  

  enum Except{
    EXCEPTION_CREATE_SOCKET,
    EXCEPTION_CONNECT,
    EXCEPTION_SEND_COMMAND,
    EXCEPTION_POLL_FAIL,
    EXCEPTION_POLL_TIMEOUT,
    EXCEPTION_PARSING_FAIL
  };

  enum Ret {
    RET_SUCCESS,
    RET_CREATE_SOCKET_FAIL,
    RET_CONNECT_FAIL,
    RET_SEND_CMD_FAIL,
    RET_POLL_FAIL,
    RET_POLL_TIMEOUT,
    RET_PARSING_FAIL
  };

  WebService(const char* ip, int port);
  ~WebService(){};
  //int start();

  //dump
  static const char* dump_error(Except e);

//request
  char* request_CodeDataSelect(const char *sMemcoCd, const char* sSiteCd, const char* sDvLoc, int timelimit, CCBFunc cbfunc, void* client);
  char* request_CodeDataSelect(const char *sMemcoCd, const char* sSiteCd, const char* sDvLoc, int timelimit)
  {
    request_CodeDataSelect(sMemcoCd, sSiteCd, sDvLoc, timelimit, NULL, NULL);
  }
  char* request_CodeDataSelect(const char *sMemcoCd, const char* sSiteCd, const char* sDvLoc, CCBFunc cbfunc, void* client)
  {
    request_CodeDataSelect(sMemcoCd, sSiteCd, sDvLoc, 0, cbfunc, client);
  }

  bool request_GetNetInfo(int timelimit, CCBFunc cbfunc, void* client);
  bool request_GetNetInfo(int timelimit)
  {
    request_GetNetInfo(timelimit, NULL, NULL);
  }
  bool request_GetNetInfo(CCBFunc cbfunc, void* client)
  {
    request_GetNetInfo(0, cbfunc, client);
  }
  void request_RfidInfoSelectAll(const char *sMemcoCd, const char* sSiteCd, int timelimit, CCBFunc cbfunc, void* client, 
  const char* outFilename);
  void request_RfidInfoSelectAll(const char *sMemcoCd, const char* sSiteCd, int timelimit, 
  const char* outFilename)
  {
    request_RfidInfoSelectAll(sMemcoCd, sSiteCd, timelimit, NULL, NULL, outFilename);
  }
  void request_RfidInfoSelectAll(const char *sMemcoCd, const char* sSiteCd, CCBFunc cbfunc, void* client, 
  const char* outFilename)
  {
    request_RfidInfoSelectAll(sMemcoCd, sSiteCd, 0, cbfunc, client, outFilename);
  }

  char* request_RfidInfoSelect(const char *sMemcoCd, const char* sSiteCd, char* serialnum, int timelimit, CCBFunc cbfunc, void* 
  client);
  char* request_RfidInfoSelect(const char *sMemcoCd, const char* sSiteCd, char* serialnum, int timelimit)
  {
    request_RfidInfoSelect(sMemcoCd, sSiteCd, serialnum, timelimit, NULL, NULL);
  }
  char* request_RfidInfoSelect(const char *sMemcoCd, const char* sSiteCd, char* serialnum, CCBFunc cbfunc, void* 
  client)
  {
    request_RfidInfoSelect(sMemcoCd, sSiteCd, serialnum, 0, cbfunc, client);
  }
  char* request_ServerTimeGet(int timelimit, CCBFunc cbfunc, void* client);
  char* request_ServerTimeGet(int timelimit)
  {
    request_ServerTimeGet(timelimit, NULL, NULL);
  }
  char* request_ServerTimeGet(CCBFunc cbfunc, void* client)
  {
    request_ServerTimeGet(0, cbfunc, client);
  }

  bool request_StatusUpdate(char *sGateType, const char* sSiteCd, const char* sDvLoc, char* sdvNo, char* sIpAddress, char* sMacAddress, int timelimit, CCBFunc cbfunc, void* 
  client);
  bool request_StatusUpdate(char *sGateType, const char* sSiteCd, const char* sDvLoc, char* sdvNo, char* sIpAddress, char* sMacAddress, int timelimit)
  {
    request_StatusUpdate(sGateType, sSiteCd, sDvLoc, sdvNo, sIpAddress, sMacAddress, timelimit, NULL, NULL);
  }
  bool request_StatusUpdate(char *sGateType, const char* sSiteCd, const char* sDvLoc, char* sdvNo, char* sIpAddress, char* sMacAddress, CCBFunc cbfunc, void* 
  client)
  {
    request_StatusUpdate(sGateType, sSiteCd, sDvLoc, sdvNo, sIpAddress, sMacAddress, 0, cbfunc, client);
  }
  
  bool request_TimeSheetInsertString(const char *sMemcoCd, const char* sSiteCd, const char* sLabNo, char cInOut, char* sGateNo, char* sGateLoc, char cUtype, char* sInTime, char* sPhotoImage, int timelimit, CCBFunc cbfunc, void* 
  client);
  bool request_TimeSheetInsertString(const char *sMemcoCd, const char* sSiteCd, const char* sLabNo, char cInOut, char* sGateNo, char* sGateLoc, char cUtype, char* sInTime, char* sPhotoImage, int timelimit)
  {
    request_TimeSheetInsertString(sMemcoCd, sSiteCd, sLabNo, cInOut, sGateNo, sGateLoc, cUtype, sInTime, sPhotoImage, timelimit, NULL, NULL);
  }
  bool request_TimeSheetInsertString(const char *sMemcoCd, const char* sSiteCd, const char* sLabNo, char cInOut, char* sGateNo, char* sGateLoc, char cUtype, char* sInTime, char* sPhotoImage, CCBFunc cbfunc, void* 
  client)
  {
    request_TimeSheetInsertString(sMemcoCd, sSiteCd, sLabNo, cInOut, sGateNo, sGateLoc, cUtype, sInTime, sPhotoImage, 0, cbfunc, client);
  }
private:
  //void run(); 
  
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

