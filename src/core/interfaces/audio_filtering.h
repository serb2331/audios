#pragma once

#include "core/context/Context.h"
#include <memory>

class IAudioFilterSource {
public:
  virtual u_int32_t readFrames(float *frameBuffer, u_int32_t numFrames) = 0;
  virtual u_int32_t getChannelNumber() = 0;
};

// Abstract class
class AudioFilterBase : public IAudioFilterSource {
private:
  std::unique_ptr<IAudioFilterSource> _pFilterSourceWrapee;

public:
  AudioFilterBase(IAudioFilterSource *filterWrapee) {
    if (!filterWrapee)
      USE_LOGGING_ERROR("Initialized AudioFilter with a null filter source.");

    _pFilterSourceWrapee.reset(filterWrapee);
  }

  virtual u_int32_t readFrames(float *frameBuffer,
                               u_int32_t numFrames) override {
    return _pFilterSourceWrapee->readFrames(frameBuffer, numFrames);
  }
  virtual u_int32_t getChannelNumber() override {
    return _pFilterSourceWrapee->getChannelNumber();
  }
};