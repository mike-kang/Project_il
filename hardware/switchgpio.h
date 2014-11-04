#ifndef _SWITCHGPIO_HEADER
#define _SWITCHGPIO_HEADER

#include "gpio.h"
#include "../tools/timer.h"

//only out direct
class SwitchGpio : private Gpio{
public:
  SwitchGpio(int num, bool init=false):Gpio(num, true, init){}
  virtual ~SwitchGpio(){};
  void on(){ write(true); }
  void on(int msec);
  void on(int*, bool repeat);
  void off(){ write(false); }

private:
  tools::Timer* m_timerOn;
  tools::Timer* m_timerOff;
};

#endif

