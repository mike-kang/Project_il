#include <iostream>
#include <fstream>
#include "base64.h"

using namespace std;

int main()
{
  //jpeg -> base64
  ifstream infile ("org.jpg",ofstream::binary);
  // get size of file
  infile.seekg (0,infile.end);
  long size = infile.tellg();
  infile.seekg (0);
  // allocate memory for file content
  char* buffer = new char[size];
  // read content of infile
  infile.read (buffer,size);
  infile.close();

  char* resultBuffer = new char[size*2];
  
  std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(buffer),size);
  //std::string decoded = base64_decode(encoded);

  std::ofstream oOut("resultbyEncode.txt");
  oOut << encoded << endl;
  oOut.close();
  


  //base64 -> jpeg
  string encoded2;
  infile.open("org_txt.txt");
  // get size of file
  getline(infile, encoded2);
  string decoded = base64_decode(encoded2);
  
  ofstream oOut2("resultbyDecode.jpg", ofstream::binary);
  oOut2 << decoded << endl;
  oOut2.close();
  return 0;
}

/*
int __main()
{
  CameraStill* m_cameraStill;

  m_cameraStill = new CameraStill(10); //10 sec
  char* imgBuf = NULL;;
  int imgLength;
  if(m_cameraStill->takePicture(&imgBuf, &imgLength, 2))  //2sec 
  {
    //for test
    if(imgLength < 0)
      cout << "takePicture error!" << endl;
    else{
      ofstream outfile ("test.jpg",ofstream::binary);
      outfile.write (imgBuf,imgLength);
      outfile.close();
    }
  }

  while(1)
    sleep(1);
  
  return 0;
}
*/
