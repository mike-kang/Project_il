#include "camerastill.h"
#include <iostream>

int main()
{
  CameraStill* m_cameraStill;

  m_cameraStill = new CameraStill(60 * 10); //10 minutes
  m_cameraStill->takePicture();

  while(1)
    sleep(1);
  
  return 0;
}

