#pragma once

#include "_external/private_embree.h" // IWYU pragma: keep
#include "src/context/Context_p.h"    // IWYU pragma: keep
#include <embree4/rtcore_device.h>
#include <embree4/rtcore_scene.h>
#include <iostream> // IWYU pragma: keep
#include <memory>

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

struct RTCDeviceDeleter {
  void operator()(RTCDevice d) const { rtcReleaseDevice(d); }
};
struct RTCSceneDeleter {
  void operator()(RTCScene s) const { rtcReleaseScene(s); }
};

using UniqueRTCDevice =
    std::unique_ptr<std::remove_pointer_t<RTCDevice>, RTCDeviceDeleter>;
using UniqueRTCScene =
    std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>;