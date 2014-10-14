#ifndef _SWITCHGPIO_HEADER
#define _SWITCHGPIO_HEADER

#include "gpio.h"

//only out direct
class SwitchGpio : private Gpio{
public:
  SwitchGpio(int num, bool init=false):Gpio(num, true, init){}
  virtual ~SwitchGpio(){};
  void on(){ write(true); }
  void off(){ write(false); }
};

#endif

