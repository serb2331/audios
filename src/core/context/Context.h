#pragma once

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
};