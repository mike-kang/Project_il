#ifndef _WAV_PLAYER_HEADER
#define _WAV_PLAYER_HEADER

namespace tools {

namespace media {
void wavPlay(const char* filename)
{
  char cmd[255];
  sprintf(cmd, "aplay %s", filename);
  system(cmd);
}

}
}
#endif //_WAV_PLAYER_HEADER

