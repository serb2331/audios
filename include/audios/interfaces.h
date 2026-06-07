#pragma once

#include "export.h"
#include <cstdint>
#include <string>

namespace audios {

class AUDIOS_EXPORT IAudioFileDecoder {
public:
  virtual bool initFile(std::string filePath) = 0;
  virtual void resetPointer() = 0;
  virtual void resetDecoder() = 0;

  virtual uint32_t readFrames(float *frameBuffer, uint32_t numFrames) = 0;
  virtual void dumpContents(uint32_t framesToDump) = 0;

  virtual void logFileInformation() = 0;
};

class AUDIOS_EXPORT IAudioFileEncoder {
public:
  virtual bool initFile(std::string filePath) = 0;
  virtual bool initFile(std::string filePath, const void *format) = 0;

  virtual bool isInitialized() const = 0;

  virtual void resetEncoder() = 0;

  virtual uint32_t writeFrames(const float *frameBuffer,
                               uint32_t numFrames) = 0;
};

class AUDIOS_EXPORT IAudioFilterSource {
public:
  virtual ~IAudioFilterSource() = default;

  virtual u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) = 0;
  virtual u_int32_t getChannelNumber() = 0;
};

} // namespace audios