#include <iostream>
#include "maindelegator.h"
#include <unistd.h>

using namespace std;

class Listener : public MainDelegator::EventListener{
  virtual void onMessage(std::string tag, std::string data);
  virtual void onEmployeeInfo(std::string CoName, std::string Name, std::string PinNo, const unsigned char* img_buf, int img_sz);
  virtual void onStatus(std::string status);
};

void Listener::onMessage(std::string tag, std::string data)
{
  cout << "onMessage\n" << endl;
}

void Listener::onEmployeeInfo(std::string CoName, std::string Name, std::string PinNo, const unsigned char* img_buf, int img_sz)
{
  cout << "onEmployeeInfo\n" << endl;
}

void Listener::onStatus(std::string status)
{
  cout << "onStatus\n" << endl;

}

int main(int argc, char *argv[])
{
  const char* config = NULL;
  
  cout << "start main\n" << endl;
  Listener* l = new Listener();
  
  int opt = getopt(argc, argv, "c:");   
  if(opt == 'c'){
    cout << config << endl;
    config = optarg;
  }
  
  MainDelegator::createInstance(l, config);

  while(1){
    cout << "Koong" << endl;
    sleep(100);
  }
  cout << "exit" << endl;
  return 0;
}
