#pragma once

#include "_external/private_embree.h" // IWYU pragma: keep
#include "src/context/Context_p.h"    // IWYU pragma: keep
#include <iostream>                   // IWYU pragma: keep
#include <memory>

#define USE_LOGGING(X)                                                         \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging())                                   \
      std::cout << "\033[32m[AudioLibrary]\033[0m " << X << std::endl;         \
  }

#define USE_LOGGING_ERROR(X)                                                   \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging())                                   \
      std::cout << "\033[31m[AudioLibrary]\033[0m " << X << std::endl;         \
  }

#define USE_EMBREE_DEVICE_ERROR(X)                                             \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging()) {                                 \
      auto error = rtcGetDeviceError(X);                                       \
      if (error != RTC_ERROR_NONE) {                                           \
        std::cout << "\033[31m[AudioLibrary]\033[0m Embree device error: "     \
                  << error << std::endl;                                       \
        std::cout << "\033[31m[AudioLibrary]\033[0m Error String: "            \
                  << rtcGetErrorString(error) << std::endl;                    \
      }                                                                        \
    }                                                                          \
  }

#define USE_EMBREE_DEVICE_ERROR_LOG(X, Y)                                      \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging()) {                                 \
      auto error = rtcGetDeviceError(X);                                       \
      if (error != RTC_ERROR_NONE) {                                           \
        std::cout << "\033[31m[AudioLibrary]\033[0m " << Y << std::endl;       \
        std::cout << "\033[31m[AudioLibrary]\033[0m Embree device error: "     \
                  << error << std::endl;                                       \
        std::cout << "\033[31m[AudioLibrary]\033[0m Error String: "            \
                  << rtcGetErrorString(error) << std::endl;                    \
      }                                                                        \
    }                                                                          \
  }

struct RTCDeviceDeleter {
  void operator()(RTCDevice d) const { rtcReleaseDevice(d); }
};
struct RTCSceneDeleter {
  void operator()(RTCScene s) const { rtcReleaseScene(s); }
};
struct RTCGeometryDeleter {
  void operator()(RTCGeometry g) const { rtcReleaseGeometry(g); }
};

using UniqueRTCDevice =
    std::unique_ptr<std::remove_pointer_t<RTCDevice>, RTCDeviceDeleter>;
using UniqueRTCScene =
    std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>;
using UniqueRTCGeometry =
    std::unique_ptr<std::remove_pointer_t<RTCGeometry>, RTCGeometryDeleter>;