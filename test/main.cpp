#include <iostream>
#include "maindelegator.h"

using namespace std;


int main()
{
  cout << "start main\n" << endl;
  MainDelegator* md = MainDelegator::getInstance();

  while(1){
    cout << "Koong" << endl;
    sleep(100);
  }
  cout << "exit" << endl;
  return 0;
}
