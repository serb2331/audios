#include "context/Context_p.h"
#include "include/audios/configuration.h"

namespace audios {

void Configuration::setLogging(bool loggingValue) {
  AudiosContext::GetInstance().setLogging(loggingValue);
}

bool Configuration::isLoggingEnabled() {
  return AudiosContext::GetInstance().getLogging();
}

void Configuration::setBufferFrameCount(uint32_t size) {
  AudiosContext::GetInstance().setBufferFrameCount(size);
}
uint32_t Configuration::getBufferFrameCount() {
  return AudiosContext::GetInstance().getBufferFrameCount();
}

} // namespace audios