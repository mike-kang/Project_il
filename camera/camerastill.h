#ifndef _CAMERA_STILL_HEADER
#define _CAMERA_STILL_HEADER

#include <iostream>

class OMXILComponent;
class OMXILEncoderComponent;
class OMXILCameraComponent;

class CameraStill {
public:
#if 0  
  const static int CAM_WIDTH = 320;
  const static int CAM_HEIGHT = 240;
#else
  const static int CAM_WIDTH = 2592;
  const static int CAM_HEIGHT = 1944;
#endif
  const static int BUF_SIZE = 1024*1024;
  
  enum Except{
    EXCEPTION_OMX_INIT,
  };

  CameraStill(int close_delay_time);

  virtual ~CameraStill();
  
  void takePicture();
  
private:  
  void run();
  bool takePictureReady();
  int m_close_delay_time; //sec
  OMXILCameraComponent* m_camera_component;
  OMXILComponent* m_preview_component;
  OMXILEncoderComponent* m_encoder_component;
  
  char m_imgBuf[BUF_SIZE];
  bool m_bReady;

  static void bufferFilled(int size);
};




#endif

