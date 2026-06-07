#include "audios/interfaces.h"
#include "include/audios/audio_filtering.h"
#include <memory>

namespace audios {

AudioFilterBase::AudioFilterBase(
    std::unique_ptr<IAudioFilterSource> filterWrapee) {
  _pFilterSourceWrapee = std::move(filterWrapee);
}
AudioFilterBase::~AudioFilterBase() = default;

u_int32_t AudioFilterBase::readFrames(float *frameBuffer, u_int32_t numFrames) {
  return _pFilterSourceWrapee->readFrames(frameBuffer, numFrames);
}

u_int32_t AudioFilterBase::getChannelNumber() {
  return _pFilterSourceWrapee->getChannelNumber();
}

} // namespace audios