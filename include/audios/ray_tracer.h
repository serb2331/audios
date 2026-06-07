#pragma once

#include "audios/geometry.h"
#include "export.h"
#include <memory>

namespace audios {

class AUDIOS_EXPORT RTEmbreeFacade {
private:
  struct RTEFacadeImpl;
  std::unique_ptr<RTEFacadeImpl> _impl;

public:
  struct Configuration {
    uint32_t rayCount;
    uint32_t maxBounce;
  };

  RTEmbreeFacade();
  RTEmbreeFacade(std::string embreeDeviceConfig, Configuration conf);
  ~RTEmbreeFacade();

  uint32_t registerGeometryFromBinaryFile(std::string filePath,
                                          uint32_t registerId);
  uint32_t instanceGeometryById(uint32_t geometrySceneId,
                                AffineTransformMatrix transform,
                                uint32_t instanceRegisterId);
  void instanceUntrackedGeometryById(uint32_t geometrySceneId,
                                     AffineTransformMatrix transform);
  uint32_t addSoundEmitterSphere(Vector3 topLevelScenePosition, float radius,
                                 uint32_t registerId);
  void testMainSceneRayTrace();
  void renderMainScene(Vector3 listenerPosition);
};

} // namespace audios