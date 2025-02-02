#pragma once

#include <fstream>
#include <sndcore2/voice.h>
#include <vector>

// only one of these can exist at a time Or Else, TODO make
// a getter
struct MusicPlayer {
  MusicPlayer(const char *oggFileName, float offsetSeconds = 0.0);
  ~MusicPlayer();
  void play();
  void pause();
  void pauseToggle(void);
  bool isPlaying();
  bool isDone();
  void seek(float seconds);
  float currentTime();

protected:
  void frameCallback(void);

private:
  bool playState;
  AXVoice *voiceLeft;
  AXVoice *voiceRight;
  uint32_t sampleRate;
  int16_t *bufferLeft;
  int16_t *bufferRight;
};
