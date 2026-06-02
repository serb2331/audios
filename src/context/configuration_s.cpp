#include "context/Context_p.h"
#include "include/audios/configuration.h"

namespace audios {

void Configuration::setLogging(bool loggingValue) {
  Context::GetInstance().setLogging(loggingValue);
}

bool Configuration::isLoggingEnabled() {
  return Context::GetInstance().getLogging();
}

void Configuration::setBufferFrameCount(uint32_t size) {
  Context::GetInstance().setBufferFrameCount(size);
}
uint32_t Configuration::getBufferFrameCount() {
  return Context::GetInstance().getBufferFrameCount();
}

} // namespace audios