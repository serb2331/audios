#pragma once

#include "_external/private_embree.h"
#include "private_macros.h"
#include <memory>

namespace audios {

class RTEmbreeSceneManager {
private:
  const RTCDevice &_mainDevice;
  std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>
      _pTopLevelScene;

public:
  RTEmbreeSceneManager(const RTCDevice &mainDevice);
  ~RTEmbreeSceneManager();
};

} // namespace audios