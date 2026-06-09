#pragma once

#include "audios/geometry.h"
#include "export.h"
#include <memory>

namespace audios {

struct alignas(16) AcousticRayTraceResult {
  float dist;
  float en;
  float freq;
  uint32_t bounceCount;
  Vector3 lastNormal;
  Vector3 startDirection;
  Vector3 finalDirection;
  uint32_t hitEmitterId;
};

static constexpr AcousticRayTraceResult INVALID_RESULT = {
    -1.0f, // or std::numeric_limits<float>::infinity()
    0.0f,  0.0f, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0};

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
  uint32_t getTracingRayCount();
  uint32_t renderMainScene(Vector3 listenerPosition,
                           AcousticRayTraceResult *resultBuffer,
                           uint32_t rayCount);
};

} // namespace audios