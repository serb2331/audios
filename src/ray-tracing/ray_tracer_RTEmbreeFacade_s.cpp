#include "_external/private_embree.h"
#include "include/audios/ray_tracer.h"
#include "private_macros.h"
#include "ray-tracing/rendering/RTEmbreeRenderingManager_p.h"
#include "scene/RTEmbreeSceneManager_p.h"
#include "utils/random_p.h"
#include <cstdint>
#include <memory>

namespace audios {

struct RTEmbreeFacade::RTEFacadeImpl {
  std::unique_ptr<std::remove_pointer_t<RTCDevice>, RTCDeviceDeleter> pDevice;
  std::unique_ptr<RTEmbreeSceneManager> pSceneManager;
  std::unique_ptr<RTEmbreeRenderingManager> pRenderingManager;
};

RTEmbreeFacade::RTEmbreeFacade() : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(NULL));
  _impl->pSceneManager =
      std::make_unique<RTEmbreeSceneManager>(_impl->pDevice.get());
  _impl->pRenderingManager = std::make_unique<RTEmbreeRenderingManager>(
      RTEmbreeRenderingManager::Configuration());

  AudiosRandomizer::Setup();
}
RTEmbreeFacade::RTEmbreeFacade(std::string embreeDeviceConfig,
                               Configuration conf)
    : _impl(std::make_unique<RTEFacadeImpl>()) {
  _impl->pDevice.reset(rtcNewDevice(embreeDeviceConfig.c_str()));
  _impl->pSceneManager =
      std::make_unique<RTEmbreeSceneManager>(_impl->pDevice.get());

  RTEmbreeRenderingManager::Configuration c = {conf.rayCount, conf.maxBounce};
  _impl->pRenderingManager = std::make_unique<RTEmbreeRenderingManager>(c);

  AudiosRandomizer::Setup();
}

RTEmbreeFacade::~RTEmbreeFacade() {}

uint32_t RTEmbreeFacade::registerGeometryFromBinaryFile(std::string filePath,
                                                        uint32_t registerId) {
  return _impl->pSceneManager->registerGeometryFromBinaryFile(filePath,
                                                              registerId);
}

uint32_t RTEmbreeFacade::registerGeometryFromBuffer(const float *vertices,
                                                    uint32_t nVertices,
                                                    const uint32_t *indexes,
                                                    uint32_t nIndexes,
                                                    uint32_t registerId) {

  return _impl->pSceneManager->registerGeometryFromBuffer(
      vertices, nVertices, indexes, nIndexes, registerId);
}

uint32_t RTEmbreeFacade::instanceGeometryById(uint32_t geometrySceneId,
                                              AffineTransformMatrix transform,
                                              uint32_t instanceRegisterId) {
  return _impl->pSceneManager->instanceGeometryFromLibrary(
      geometrySceneId, transform, instanceRegisterId);
}

uint32_t RTEmbreeFacade::addSoundEmitterSphere(Vector3 topLevelScenePosition,
                                               float radius,
                                               uint32_t registerId) {
  return _impl->pSceneManager->addSoundEmitterSphere(topLevelScenePosition,
                                                     radius, registerId);
}

void RTEmbreeFacade::testMainSceneRayTrace() {
  _impl->pRenderingManager->rayHitTestScene(
      _impl->pSceneManager->getMainScene());
}

uint32_t RTEmbreeFacade::getTracingRayCount() {
  return _impl->pRenderingManager->getRayCount();
}

uint32_t RTEmbreeFacade::renderMainScene(Vector3 listenerPosition,
                                         AcousticRayTraceResult *resultBuffer,
                                         uint32_t rayCount) {
  return _impl->pRenderingManager->renderScene(
      _impl->pSceneManager->getMainScene(), listenerPosition,
      _impl->pSceneManager->getEmitterIds(), resultBuffer, rayCount);
}

} // namespace audios