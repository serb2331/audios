#pragma once

#include <cstdint>
#include <iostream> // IWYU pragma: keep

namespace audios {

class Context {
private:
  Context();

  bool _logging;
  uint32_t _defaultBufferFrameCount = 512;

public:
  static Context &GetInstance();

  Context(Context &other) = delete;
  void operator=(const Context &other) = delete;

  void setLogging(bool loggingValue);
  bool getLogging();

  void setBufferFrameCount(uint32_t size);
  uint32_t getBufferFrameCount();
};

} // namespace audios