#pragma once

#include "audios/interfaces.h"
#include <memory>

namespace audios {

// Abstract class
class AUDIOS_EXPORT AudioFilterBase : public IAudioFilterSource {
private:
  std::unique_ptr<IAudioFilterSource> _pFilterSourceWrapee;

public:
  AudioFilterBase(std::unique_ptr<IAudioFilterSource> filterWrapee);
  virtual ~AudioFilterBase();

  virtual u_int32_t readFrames(float *frameBuffer,
                               u_int32_t numFrames) override;
  virtual u_int32_t getChannelNumber() override;
};

class AUDIOS_EXPORT GainAudioFilter : public AudioFilterBase {
private:
  float _gainPercent;

public:
  GainAudioFilter(float gainPercent,
                  std::unique_ptr<IAudioFilterSource> filterWrapee);

  virtual u_int32_t readFrames(float *frameBuffer,
                               u_int32_t numFrames) override;

  bool execute(u_int32_t numFrames, IAudioFileEncoder &destination);
};

} // namespace audios