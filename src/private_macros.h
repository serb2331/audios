#pragma once

#include "_external/private_embree.h" // IWYU pragma: keep
#include "src/context/Context_p.h"    // IWYU pragma: keep
#include <iostream>                   // IWYU pragma: keep
#include <memory>

#include <sstream>

#define USE_LOGGING(X)                                                         \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging() &&                           \
        AudiosContext::GetInstance().getLogCallback()) {                       \
      std::ostringstream oss;                                                  \
      oss << "[AudioLibrary] " << X;                                           \
      AudiosContext::GetInstance().getLogCallback()(oss.str().c_str());        \
    }                                                                          \
  }

#define USE_LOGGING_ERROR(X)                                                   \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging() &&                           \
        AudiosContext::GetInstance().getLogCallback()) {                       \
      std::ostringstream oss;                                                  \
      oss << "[AudioLibrary] " << X;                                           \
      AudiosContext::GetInstance().getLogCallback()(oss.str().c_str());        \
    }                                                                          \
  }

#define USE_EMBREE_DEVICE_ERROR(X)                                             \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging() &&                           \
        AudiosContext::GetInstance().getLogCallback()) {                       \
      auto error = rtcGetDeviceError(X);                                       \
      if (error != RTC_ERROR_NONE) {                                           \
        std::ostringstream oss;                                                \
        oss << "[AudioLibrary] Embree device error: " << error                 \
            << "\n[AudioLibrary] Error String: " << rtcGetErrorString(error);  \
        AudiosContext::GetInstance().getLogCallback()(oss.str().c_str());      \
      }                                                                        \
    }                                                                          \
  }

#define USE_EMBREE_DEVICE_ERROR_LOG(X, Y)                                      \
  {                                                                            \
    if (AudiosContext::GetInstance().getLogging() &&                           \
        AudiosContext::GetInstance().getLogCallback()) {                       \
      auto error = rtcGetDeviceError(X);                                       \
      if (error != RTC_ERROR_NONE) {                                           \
        std::ostringstream oss;                                                \
        oss << "[AudioLibrary] " << Y                                          \
            << "\n[AudioLibrary] Embree device error: " << error               \
            << "\n[AudioLibrary] Error String: " << rtcGetErrorString(error);  \
        AudiosContext::GetInstance().getLogCallback()(oss.str().c_str());      \
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