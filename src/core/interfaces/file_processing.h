#pragma once

#include "core/interfaces/audio_filtering.h"
#include <string>
#include <sys/types.h>

class IAudioFileCodec : public IAudioFilterSource {
public:
  virtual bool openFile(std::string filePath) = 0;

  virtual void reset() = 0;

  virtual void logFileInformation() = 0;

  virtual void dumpContents(u_int32_t framesToDump) = 0;
};