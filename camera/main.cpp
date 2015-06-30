#include "camerastill.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "memory.h"
#include "log.h"

using namespace std;
int main()
{
  //log_init(true, 1, "/dev/pts/2", false, 3, "Log");

  CameraStill* m_cameraStill;
  cout << "###0memory " <<tools::memory::getHeapSize() << endl;

  m_cameraStill = new CameraStill(40); //10 sec
  char* imgBuf = NULL;;
  int imgLength;
  
  cout << "###1memory " <<tools::memory::getHeapSize() << endl;
  if(m_cameraStill->takePicture(&imgBuf, &imgLength, 5))  //2sec 
  {
    //for test
    if(imgLength < 0)
      cout << "takePicture error!";
    else{
      ofstream outfile ("test.jpg",ofstream::binary);
      outfile.write (imgBuf,imgLength);
      outfile.close();
    }
  }
  sleep(4);
  cout << "###2memory " <<tools::memory::getHeapSize() << endl;
  if(m_cameraStill->takePicture(&imgBuf, &imgLength, 5))  //2sec 
  {
    //for test
    if(imgLength < 0)
      cout << "takePicture error!";
    else{
      ofstream outfile ("test.jpg",ofstream::binary);
      outfile.write (imgBuf,imgLength);
      outfile.close();
    }
  }
  sleep(4);
  cout << "###3memory " <<tools::memory::getHeapSize() << endl;
  delete m_cameraStill;
  cout << "###4memory " <<tools::memory::getHeapSize() << endl;
  
  return 0;
}

