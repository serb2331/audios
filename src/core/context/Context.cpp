#include "Context.h"

Context::Context() {}

Context &Context::GetInstance() {
  static Context instance;
  return instance;
}

void Context::setLogging(bool loggingValue) { _logging = loggingValue; }

bool Context::getLogging() { return _logging; }

void Context::setBufferSize(u_int32_t size) { _defaultBufferSize = size; }
u_int32_t Context::getBufferSize() { return _defaultBufferSize; }