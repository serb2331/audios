#include "Context_p.h"

namespace audios {

Context::Context() {}

Context &Context::GetInstance() {
  static Context instance;
  return instance;
}

void Context::setLogging(bool loggingValue) { _logging = loggingValue; }

bool Context::getLogging() { return _logging; }

void Context::setBufferFrameCount(uint32_t size) {
  _defaultBufferFrameCount = size;
}
uint32_t Context::getBufferFrameCount() { return _defaultBufferFrameCount; }

} // namespace audios