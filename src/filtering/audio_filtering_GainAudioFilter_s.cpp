#include "include/audios/audio_filtering.h"
#include "private_macros.h"
#include <vector>

namespace audios {

GainAudioFilter::GainAudioFilter(
    float gainPercent, std::unique_ptr<IAudioFilterSource> filterWrapee)
    : AudioFilterBase(std::move(filterWrapee)), _gainPercent(gainPercent) {}

u_int32_t GainAudioFilter::readFrames(float *frameBuffer, u_int32_t numFrames) {
  // USE_LOGGING("GainFilter reading " << numFrames << " from wrapped source.");
  auto readFrames = AudioFilterBase::readFrames(frameBuffer, numFrames);

  for (u_int32_t i = 0; i < readFrames * AudioFilterBase::getChannelNumber();
       i += 1) {
    frameBuffer[i] *= _gainPercent;
  }

  return readFrames;
}

bool GainAudioFilter::execute(u_int32_t numFrames,
                              IAudioFileEncoder &destination) {
  if (!destination.isInitialized()) {
    USE_LOGGING_ERROR(
        "Can't execute filter on an uninitialized destination encoder.");
    return false;
  }

  auto channelCount = AudioFilterBase::getChannelNumber();
  std::vector<float> filteringBuffer(numFrames * channelCount, 0.0);
  u_int32_t nrFramesRead = readFrames(filteringBuffer.data(), numFrames);

  while (nrFramesRead == numFrames) {
    destination.writeFrames(filteringBuffer.data(), nrFramesRead);
    nrFramesRead = readFrames(filteringBuffer.data(), numFrames);
  }

  if (nrFramesRead > 0)
    destination.writeFrames(filteringBuffer.data(), nrFramesRead);

  return true;
}

} // namespace audios