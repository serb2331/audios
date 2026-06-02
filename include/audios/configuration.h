#pragma once

#include "include/audios/export.h"
#include <cstdint>

namespace audios {

class AUDIOS_EXPORT Configuration {
public:
  Configuration() = delete;

  static void setLogging(bool loggingValue);
  static bool isLoggingEnabled();

  static void setBufferFrameCount(uint32_t size);
  static uint32_t getBufferFrameCount();
};

} // namespace audios