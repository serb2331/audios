#include "Context_p.h"
#include "utils/random_p.h"

namespace audios {

AudiosContext::AudiosContext() {}

AudiosContext &AudiosContext::GetInstance() {
  static AudiosContext instance;
  return instance;
}

void AudiosContext::setLogging(bool loggingValue) { _logging = loggingValue; }

bool AudiosContext::getLogging() { return _logging; }

void AudiosContext::setBufferFrameCount(uint32_t size) {
  _defaultBufferFrameCount = size;
}
uint32_t AudiosContext::getBufferFrameCount() {
  return _defaultBufferFrameCount;
}

void AudiosContext::setRayCount(uint32_t rayCount) {
  _renderingRayCount = rayCount;
}
uint32_t AudiosContext::getRayCount() { return _renderingRayCount; }

void AudiosContext::setMaxBounceCount(uint32_t maxBounceCount) {
  _maxBounceCount = maxBounceCount;
}
uint32_t AudiosContext::getMaxBounceCount() { return _maxBounceCount; }

} // namespace audios