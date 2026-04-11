#include "Context.h"
#include <iostream>
#include <ostream>

Context::Context() {
}

Context& Context::GetInstance()
{
    static Context instance;
    return instance;
}


void Context::setLogging(bool loggingValue)
{
    _logging = loggingValue;
}

bool Context::getLogging() {return _logging;}