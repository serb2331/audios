#include "Context.h"

Context::Context() {}

Context &Context::GetInstance() {
  static Context instance;
  return instance;
}

void Context::setLogging(bool loggingValue) { _logging = loggingValue; }

bool Context::getLogging() { return _logging; }

void Context::setBufferFrameCount(u_int32_t size) {
  _defaultBufferFrameCount = size;
}
u_int32_t Context::getBufferFrameCount() { return _defaultBufferFrameCount; }