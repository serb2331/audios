#pragma once

#include <cstdint>
#include <iostream> // IWYU pragma: keep

namespace audios {

class AudiosContext {
private:
  AudiosContext();

  bool _logging;
  uint32_t _defaultBufferFrameCount = 512;
  uint32_t _renderingRayCount = 100;
  uint32_t _maxBounceCount = 100;

public:
  static AudiosContext &GetInstance();

  AudiosContext(AudiosContext &other) = delete;
  void operator=(const AudiosContext &other) = delete;

  void setLogging(bool loggingValue);
  bool getLogging();

  void setBufferFrameCount(uint32_t size);
  uint32_t getBufferFrameCount();

  void setRayCount(uint32_t rayCount);
  uint32_t getRayCount();

  void setMaxBounceCount(uint32_t rayCount);
  uint32_t getMaxBounceCount();
};

} // namespace audios