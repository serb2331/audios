#pragma once

#include <string>
#include <sys/types.h>

class IAudioFileDecoder {
public:
  virtual bool initFile(std::string filePath) = 0;
  virtual void resetPointer() = 0;
  virtual void resetDecoder() = 0;

  virtual u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) = 0;
  virtual void dumpContents(u_int32_t framesToDump) = 0;

  virtual void logFileInformation() = 0;
};

class IAudioFileEncoder {
public:
  virtual bool isInitialized() const = 0;

  virtual bool initFile(std::string filePath) = 0;
  virtual bool initFile(std::string filePath, const void *format) = 0;

  virtual void resetEncoder() = 0;

  virtual u_int32_t writeFrames(const float *frameBuffer,
                                u_int32_t numFrames) = 0;
};