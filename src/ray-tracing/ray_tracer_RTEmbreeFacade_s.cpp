#include "_external/private_embree.h"
#include "include/audios/ray_tracer.h"
#include "private_macros.h"
#include <embree4/rtcore_device.h>
#include <memory>

namespace audios {

struct RTEmbreeFacade::RTEFacadeImpl {
  std::unique_ptr<std::remove_pointer_t<RTCDevice>, RTCDeviceDeleter> pDevice;
};

RTEmbreeFacade::RTEmbreeFacade() : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(NULL));
}
RTEmbreeFacade::RTEmbreeFacade(std::string config)
    : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(config.c_str()));
}

RTEmbreeFacade::~RTEmbreeFacade() {}

} // namespace audios