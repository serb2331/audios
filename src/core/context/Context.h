#pragma once

#include <iostream>

#define USE_LOGGING(X)                                                         \
  {                                                                            \
    if (Context::GetInstance().getLogging())                                   \
      std::cout << "\033[32m[AudioLibrary]\033[0m " << X << std::endl;         \
  }

#define USE_LOGGING_ERROR(X)                                                   \
  {                                                                            \
    if (Context::GetInstance().getLogging())                                   \
      std::cout << "\033[31m[AudioLibrary]\033[0m " << X << std::endl;         \
  }

class Context {
private:
  Context();

  bool _logging;

public:
  static Context &GetInstance();

  Context(Context &other) = delete;
  void operator=(const Context &other) = delete;

  void setLogging(bool loggingValue);
  bool getLogging();
};