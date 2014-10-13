#include <iostream>
#include <fstream>
#include "webservice.h"

using namespace std;

int main()
{
  WebService* m_ws;

  m_ws = new WebService("192.168.0.7", 8080);
  m_ws->start();

  int sock;
  sock = m_ws->request_GetNetInfo(3000);  //blocked I/O
  if(sock < 0)
    cout << "request_GetNetInfo fail!" << endl;
  char buf[4096+1];
  int readlen;
  if(sock> 0){
    readlen = recv(sock, buf, 4096, 0);
    cout << buf << endl;
    close(sock);
    
    //debug start
    std::ofstream oOut("received_GetNetInfo.txt");
    oOut << buf << endl;
    oOut.close();
    //debug end
  }



  return 0;
}

