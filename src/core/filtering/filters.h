#pragma once

#include "core/interfaces/audio_filtering.h"

class GainAudioFilter : public AudioFilterBase {
private:
  float _gainPercent;

public:
  GainAudioFilter(float gainPercent,
                  std::unique_ptr<IAudioFilterSource> filterWrapee);

  virtual u_int32_t readFrames(float *frameBuffer,
                               u_int32_t numFrames) override;
};