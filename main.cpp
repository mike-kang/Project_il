#include "tools/log.h"
#include <iostream>
#include "maindelegator.h"

#define LOG_TAG "main"
int main()
{
  log_init(TYPE_CONSOLE, NULL);
  LOGI("start main\n");

  MainDelegator* md = new MainDelegator();

  while(1){
    LOGI("Koong\n");
    sleep(100);
  }
  LOGI("ccc\n");
  return 0;
}
