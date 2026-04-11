#pragma once

#include <iostream>

#define USE_LOGGING(X)                              \
{                                                   \
    if (Context::GetInstance().getLogging())        \
    std::cout << "AudioLibrary: " << X << std::endl;\
}                                           

class Context
{
private:
    Context();

    bool _logging;

public:

    static Context& GetInstance();

    // Context can't be copied
    Context(Context &other) = delete;
    // COntext can't be assigned
    void operator=(const Context &other) = delete;

    void setLogging(bool loggingValue);
    bool getLogging();
};