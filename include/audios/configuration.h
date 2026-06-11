#pragma once

#include "audios/export.h"
#include <cstdint>

namespace audios {

typedef void (*LogCallback)(const char *);

class AUDIOS_EXPORT Configuration {
public:
  Configuration() = delete;

  static void setLogging(bool loggingValue);
  static bool isLoggingEnabled();
  static void setLogCallback(LogCallback callback);

  static void setBufferFrameCount(uint32_t size);
  static uint32_t getBufferFrameCount();
};

} // namespace audios