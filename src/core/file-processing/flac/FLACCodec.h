#pragma once

#include "../../../external/dr_flac.h"
#include "core/interfaces/audio_filtering.h"
#include "core/interfaces/file_processing.h"

class FLACAudioFileDecoder final : public IAudioFileDecoder,
                                   public IAudioFilterSource {
private:
  drflac *_drflacP = nullptr;
  bool _isFileInitialized = false;

public:
  bool initFile(std::string filePath) override;

  void resetPointer() override;

  void logFileInformation() override;

  void dumpContents(u_int32_t framesToDump) override;

  u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) override;

  u_int32_t getChannelNumber() override;

  ~FLACAudioFileDecoder();
};