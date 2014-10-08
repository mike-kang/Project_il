#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <bcm_host.h>
#include <interface/vcos/vcos.h>
#include "camerastill.h"
#include "log.h"
#include "dump.h"
#include "omxilComponent.h"

#define LOG_TAG "CameraStill"

#define OMX_INIT_STRUCTURE(a) \
  memset (&(a), 0, sizeof (a)); \
  (a).nSize = sizeof (a); \
  (a).nVersion.nVersion = OMX_VERSION; \
  (a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
  (a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
  (a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
  (a).nVersion.s.nStep = OMX_VERSION_STEP

#define NULLSINK_COMPONENT_NAME "OMX.broadcom.null_sink"

//static int verbos = 1;

CameraStill::CameraStill(int close_delay_time):m_close_delay_time(close_delay_time), m_bReady(false)
{
  OMX_ERRORTYPE error;
  //Initialize Broadcom's VideoCore APIs
  bcm_host_init ();
  
  //Initialize OpenMAX IL
  if ((error = OMX_Init ())){
    LOGE("error: OMX_Init:%d\n", error);
    throw EXCEPTION_OMX_INIT;
  }   

  
  m_encoder_component = new OMXILEncoderComponent(CAM_WIDTH, CAM_HEIGHT, m_imgBuf, BUF_SIZE, bufferFilled);
  m_encoder_component->set_output_port();;
  m_encoder_component->set_jpeg_settings();
  m_encoder_component->change_state(OMX_StateIdle); //blocking

  m_preview_component = new OMXILComponent(NULLSINK_COMPONENT_NAME);
  m_preview_component->change_state(OMX_StateIdle); //blocking

  LOGV("CameraStill Constructed\n");
}

CameraStill::~CameraStill()
{
  m_encoder_component->disable_port(OMXILEncoderComponent::OUTPUT_PORT_NUM);
  m_preview_component->change_state(OMX_StateLoaded); //blocking
  m_encoder_component->change_state(OMX_StateLoaded); //blocking

  delete m_preview_component;
  delete m_encoder_component;

  OMX_ERRORTYPE error;
  if ((error = OMX_Deinit ())){
    LOGE("error: OMX_Deinit: %s\n", dump_OMX_ERRORTYPE (error));
  }
  
  //Deinitialize Broadcom's VideoCore APIs
  bcm_host_deinit ();

}

//static //this function is called from encoderComponent thread.
void CameraStill::bufferFilled(int size)
{
  LOGI("bufferFilled %d\n", size);
}

void CameraStill::takePicture()
{
  takePictureReady();
  m_camera_component->capture(true);
  m_encoder_component->capture();
  
}

bool CameraStill::takePictureReady()
{
  if(m_bReady) return true;
  
  m_camera_component = new OMXILCameraComponent(CAM_WIDTH, CAM_HEIGHT);
  m_camera_component->load_camera_drivers();
  
  m_camera_component->change_state(OMX_StateIdle); //blocking

  m_camera_component->configure_sensor_settings();
  m_camera_component->set_output_port();
  m_camera_component->set_camera_settings();

  //Setup tunnels: camera (still) -> image_encode, camera (preview) -> null_sink
  if(!OMXILComponent::connect(m_camera_component, OMXILCameraComponent::OUTPUT_CAPTURE_PORT_NUM, m_encoder_component, OMXILEncoderComponent::INPUT_PORT_NUM)){
    return false;
  }
  if(!OMXILComponent::connect(m_camera_component, OMXILCameraComponent::OUTPUT_PREVIEW_PORT_NUM, m_preview_component, 240)){
    return false;
  }

  //Enable the tunnel ports
  m_camera_component->enable_port (OMXILCameraComponent::OUTPUT_CAPTURE_PORT_NUM);
  m_camera_component->enable_port (OMXILCameraComponent::OUTPUT_PREVIEW_PORT_NUM);
  m_preview_component->enable_port (240);
  m_encoder_component->enable_port (OMXILEncoderComponent::INPUT_PORT_NUM);
  m_encoder_component->enable_port (OMXILEncoderComponent::OUTPUT_PORT_NUM);

  LOGV("Change state to EXECUTING\n");
  //Change state to EXECUTING
  m_camera_component->change_state(OMX_StateExecuting); //blocking
  m_preview_component->change_state(OMX_StateExecuting); //blocking
  m_encoder_component->change_state(OMX_StateExecuting); //blocking

  m_bReady = true;
  return true;
}


