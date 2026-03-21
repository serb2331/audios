#include "Context.h"
#include <iostream>
#include <ostream>

Context::Context() {

    std::cout << "constructor for context called" << std::endl;
}

Context& Context::GetInstance()
{
    static Context instance;
    return instance;
}


void Context::setLogging(bool loggingValue)
{
    std::cout << "setting logging: " << loggingValue << std::endl;
    _logging = loggingValue;
}