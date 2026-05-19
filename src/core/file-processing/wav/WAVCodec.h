#pragma once

#include "../../../external/dr_wav.h"
#include "core/interfaces/audio_filtering.h"
#include "core/interfaces/file_processing.h"
#include <memory>

class WAVAudioFileDecoder final : public IAudioFileDecoder,
                                  public IAudioFilterSource {
private:
  std::unique_ptr<drwav> _pdrwav;
  std::string _filePath;
  bool _isFileInitialized = "";

public:
  bool initFile(std::string filePath) override;
  void resetPointer() override;
  void resetDecoder() override;
  void logFileInformation() override;
  void dumpContents(u_int32_t framesToDump) override;
  drwav_data_format getDataFormat();

  u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) override;
  u_int32_t getChannelNumber() override;

  ~WAVAudioFileDecoder();
};

class WAVAudioFileEncoder final : public IAudioFileEncoder {
private:
  std::unique_ptr<drwav> _pdrwav = nullptr;
  bool _isFileInitialized = false;
  std::string _filePath = "";

public:
  static drwav_data_format DefaultDataFormat();

  bool isInitialized() const override;

  bool initFile(std::string filePath) override;
  bool initFile(std::string filePath, const void *format) override;

  void resetEncoder() override;
  u_int32_t writeFrames(const float *frameBuffer, u_int32_t numFrames) override;

  void logFileInformation();

  ~WAVAudioFileEncoder();
};