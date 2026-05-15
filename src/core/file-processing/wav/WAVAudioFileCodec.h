#pragma once

#include "../../../external/dr_wav.h"
#include "core/interfaces/audio_filtering.h"
#include "core/interfaces/file_processing.h"
#include <memory>

class WAVAudioFileCodec final : public IAudioFileCodec,
                                public IAudioFilterSource {
private:
  std::unique_ptr<drwav> _drwavP;
  bool _isFileInitialized = false;

public:
  bool openFile(std::string filePath) override;

  void reset() override;

  void logFileInformation() override;

  void dumpContents(u_int32_t framesToDump) override;

  //

  u_int32_t getChannelNumber() override final;
  u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) override final;

  ~WAVAudioFileCodec();
};