#pragma once

#include "core/interfaces/audio_filtering.h"
#include "core/interfaces/file_processing.h"
#include <memory>
#include <sys/types.h>

typedef struct {
  u_int32_t container; /* RIFF, W64. - Check from dr_wav lib */
  u_int32_t format;    /* DR_WAVE_FORMAT_* - Check from dr_wav lib */
  u_int32_t channels;
  u_int32_t sampleRate;
  u_int32_t bitsPerSample;
} WAVAudioFileFormat;

class WAVAudioFileDecoder final : public IAudioFileDecoder,
                                  public IAudioFilterSource {
private:
  struct WAVAudioFileDecoderImpl;
  std::unique_ptr<WAVAudioFileDecoderImpl> _impl;

public:
  WAVAudioFileDecoder();
  ~WAVAudioFileDecoder();

  bool initFile(std::string filePath) override;
  void resetPointer() override;
  void resetDecoder() override;
  void logFileInformation() override;
  void dumpContents(u_int32_t framesToDump) override;
  WAVAudioFileFormat getDataFormat();

  u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) override;
  u_int32_t getChannelNumber() override;
};

class WAVAudioFileEncoder final : public IAudioFileEncoder {
private:
  struct WAVAudioFileEncoderImpl;
  std::unique_ptr<WAVAudioFileEncoderImpl> _impl;

public:
  static WAVAudioFileFormat DefaultDataFormat();

  WAVAudioFileEncoder();
  ~WAVAudioFileEncoder();

  bool isInitialized() const override;

  bool initFile(std::string filePath) override;
  bool initFile(std::string filePath, const void *format) override;

  void resetEncoder() override;
  u_int32_t writeFrames(const float *frameBuffer, u_int32_t numFrames) override;

  void logFileInformation();
};