#include "RTEmbreeSceneManager_p.h"
#include "_external/private_embree.h" // IWYU pragma: keep
#include <embree4/rtcore_device.h>
#include <embree4/rtcore_scene.h>
#include <memory>

namespace audios {

RTEmbreeSceneManager::RTEmbreeSceneManager(const RTCDevice &mainDevice)
    : _mainDevice(mainDevice) {
  _pTopLevelScene.reset(rtcNewScene(mainDevice));
}

} // namespace audios