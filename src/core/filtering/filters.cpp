#include "./filters.h"

GainAudioFilter::GainAudioFilter(
    float gainPercent, std::unique_ptr<IAudioFilterSource> filterWrapee)
    : AudioFilterBase(std::move(filterWrapee)), _gainPercent(gainPercent) {}

u_int32_t GainAudioFilter::readFrames(float *frameBuffer, u_int32_t numFrames) {
  auto readFrames = AudioFilterBase::readFrames(frameBuffer, numFrames);

  for (u_int32_t i = 0; i < readFrames * AudioFilterBase::getChannelNumber();
       i += 1) {
    frameBuffer[i] *= _gainPercent;
  }

  return readFrames;
}