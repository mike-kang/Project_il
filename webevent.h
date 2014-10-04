#ifndef _WEB_EVENT_HEADER
#define _WEB_EVENT_HEADER

//#include <iostream>

//using namespace std;

//EVENT_DATA_STRUCTURE LIST
//from Internal  
#define EV_REQ_CODE_DATE_SELECT 1
#define EV_REQ_GET_NET_INFO 2
#define EV_REQ_RFID_INFO_SELECT 3
#define EV_REQ_SERVER_TIME_GET 4
#define EV_REQ_STAT_UPDATE 5
#define EV_REQ_TIME_SHEET_INSERT 6

#if 1
template<typename T>
struct TEvent {
  typedef  void (T::*TProcessFunc)(void*); //event�� ó���ϴ� function.
  typedef  void (*CCBFunc)(int retCode, void *data, void *client_data);
  //TEvent(int val): ev_id(val), ev_data(NULL), ev_client(NULL), ev_callback(NULL){};
  //TEvent(int val, void* p): ev_id(val), ev_data(p), ev_client(NULL), ev_callback(NULL){};
  TEvent(TProcessFunc processFunc, void* data): ev_processFunc(processFunc), ev_data(data){};
  TEvent(TProcessFunc processFunc, void* data, void *client, CCBFunc ccallback): ev_processFunc(processFunc), ev_data(data), ev_client(client), 
  ev_ccallback(ccallback){};
  //void seTProcessFunc( CBFunc chfunc, int arg);

  //int ev_id;
  
  //T *ev_target; //this
  TProcessFunc ev_processFunc; //event�� ó���ϴ� function.
  void* ev_data;
  
  void *ev_client;
  CCBFunc ev_ccallback;
  //int ev_result;
};

#else
template<typename T, typename C>
struct TEvent {
  typedef  void (T::*TProcessFunc)(void*, int* /*result*/);
  typedef  void (C::*CCBFunc)(int /*result*/);
  //TEvent(int val): ev_id(val), ev_data(NULL), ev_client(NULL), ev_callback(NULL){};
  //TEvent(int val, void* p): ev_id(val), ev_data(p), ev_client(NULL), ev_callback(NULL){};
  TEvent(TProcessFunc tcallback, void* data): ev_processFunc(tcallback), ev_data(data){};
  TEvent(TProcessFunc tcallback, void* data, C *client, CCBFunc ccallback): ev_processFunc(tcallback), ev_data(data), ev_client(client), 
  ev_ccallback(ccallback){};
  //void seTProcessFunc( CBFunc chfunc, int arg);

  //int ev_id;
  
  //T *ev_target; //this
  TProcessFunc ev_processFunc;
  void* ev_data;
  
  C *ev_client;
  CCBFunc ev_ccallback;
  int ev_result;
};
#endif
template<typename T>
struct TEvent1 {
  typedef  void (T::*TProcessFunc)(void*);
  TEvent1(int id, TProcessFunc tcallback, void* data = NULL): ev_id(id), ev_processFunc(tcallback), ev_data(data){};
  //TEvent1(int id, TProcessFunc tcallback): ev_id(id), ev_processFunc(tcallback), ev_data(NULL){};

  int ev_id;
  TProcessFunc ev_processFunc;
  void* ev_data;
  
};

struct Event {
  Event(int val): ev_id(val), ev_data(NULL){};
  Event(int val, void* p): ev_id(val), ev_data(p){};

  int ev_id;
  void* ev_data;
};


#endif //_WEB_EVENT_HEADER