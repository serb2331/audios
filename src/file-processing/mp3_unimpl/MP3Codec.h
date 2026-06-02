#pragma once

#include "src/external/dr_mp3.h"
#include "src/interfaces/audio_filtering.h"
#include "src/interfaces/file_processing.h"
#include <memory>

class MP3AudioFileDecoder final : public IAudioFileDecoder,
                                  public IAudioFilterSource {
private:
  std::unique_ptr<drmp3> _drmp3P;
  bool _isFileInitialized = false;

public:
  bool initFile(std::string filePath) override;

  void resetPointer() override;

  void logFileInformation() override;

  void dumpContents(u_int32_t framesToDump) override;

  u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) override;

  u_int32_t getChannelNumber() override;

  ~MP3AudioFileDecoder();
};