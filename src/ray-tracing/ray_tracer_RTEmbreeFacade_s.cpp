#include "_external/private_embree.h"
#include "include/audios/ray_tracer.h"
#include "private_macros.h"
#include "ray-tracing/RTEmbreeSceneManager_p.h"
#include <memory>

namespace audios {

struct RTEmbreeFacade::RTEFacadeImpl {
  std::unique_ptr<std::remove_pointer_t<RTCDevice>, RTCDeviceDeleter> pDevice;
  std::unique_ptr<RTEmbreeSceneManager> pSceneManager;
};

RTEmbreeFacade::RTEmbreeFacade() : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(NULL));
  _impl->pSceneManager =
      std::make_unique<RTEmbreeSceneManager>(_impl->pDevice.get());
}
RTEmbreeFacade::RTEmbreeFacade(std::string config)
    : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(config.c_str()));
  _impl->pSceneManager =
      std::make_unique<RTEmbreeSceneManager>(_impl->pDevice.get());
}

RTEmbreeFacade::~RTEmbreeFacade() {}

bool RTEmbreeFacade::registerGeometryFromBinaryFile(std::string filePath) {
  return _impl->pSceneManager->registerGeometryFromBinaryFile(filePath);
}

} // namespace audios