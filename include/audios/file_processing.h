#pragma once

#include "audios/export.h"
#include "interfaces.h"
#include <cstdint>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>

namespace audios {

typedef struct AUDIOS_EXPORT WAVAudioFileFormat {
  uint32_t container; /* RIFF, W64. - Check from dr_wav lib */
  uint32_t format;    /* DR_WAVE_FORMAT_* - Check from dr_wav lib */
  uint32_t channels;
  uint32_t sampleRate;
  uint32_t bitsPerSample;
} WAVAudioFileFormat;

class AUDIOS_EXPORT WAVAudioFileDecoder final : public IAudioFileDecoder,
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
  void dumpContents(uint32_t framesToDump) override;
  WAVAudioFileFormat getDataFormat();

  uint32_t readFrames(float *frameBuffer, uint32_t numFrames) override;
  uint32_t getChannelNumber() override;
};

class AUDIOS_EXPORT WAVAudioFileEncoder final : public IAudioFileEncoder {
private:
  struct WAVAudioFileEncoderImpl;
  std::unique_ptr<WAVAudioFileEncoderImpl> _impl;

public:
  static WAVAudioFileFormat DefaultDataFormat();

  WAVAudioFileEncoder();
  ~WAVAudioFileEncoder();

  bool initFile(std::string filePath) override;
  bool initFile(std::string filePath, const void *format) override;

  void logFileInformation();
  bool isInitialized() const override;

  void resetEncoder() override;

  uint32_t writeFrames(const float *frameBuffer, uint32_t numFrames) override;
};

///

typedef struct AUDIOS_EXPORT Vertex3 {
  float x;
  float y;
  float z;
} Vertex3;

class AUDIOS_EXPORT BinaryGeometryReader {
private:
  std::ifstream _vertexFilePointer;
  std::ifstream _indexFilePointer;
  int64_t _vertexArraySize;
  int64_t _indexArraySize;
  std::string _filePath;

public:
  BinaryGeometryReader();
  ~BinaryGeometryReader();

  bool openFile(std::string filePath);

  void resetReader();

  int64_t getVertexCount();
  int64_t getIndexCount();

  std::optional<std::vector<Vertex3>> readVertices();
  std::optional<std::vector<uint32_t>> readIndexes(uint32_t indexCount);
};

} // namespace audios