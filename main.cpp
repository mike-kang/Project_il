#include "tools/log.h"
#include <iostream>
#include "maindelegator.h"

#define LOG_TAG "main"
int main()
{
  log_init(TYPE_CONSOLE, NULL);
  LOGI("aaaa\n");
  LOGI("bbb\n");
  sleep(1);

  MainDelegator* md = new MainDelegator();

  while(1){
    LOGI("Koong\n");
    sleep(100);
  }
  LOGI("ccc\n");
  return 0;
}
