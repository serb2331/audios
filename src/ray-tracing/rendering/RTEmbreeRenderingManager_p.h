#pragma once

#include "_external/private_embree.h"
#include "audios/geometry.h"
#include "audios/ray_tracer.h"
#include "context/Context_p.h"
#include <cstdint>
#include <memory>
#include <unordered_set>

namespace audios {

class RTEmbreeRenderingManager {
private:
  std::unique_ptr<RTCRayQueryContext> _pRayQueryContext;
  std::unique_ptr<RTCIntersectArguments> _pIntersectArguments;

  uint32_t _rayCount;
  uint32_t _maxBounce;

  struct alignas(16) AudioRayContext {
    RTCRayQueryContext rayQueryContext;
    float dist;
    float en;
    float freq;
    uint32_t bounceCount;
  };

  RTCRayHit _createInitialRayHit(Vector3 position, Vector3 direction);
  void _setupRayForTrace(RTCRayHit &rh, Vector3 newStart, Vector3 newDir);
  AudioRayContext _initRayContext(float rayFrequency, float initialEnergy);

public:
  struct Configuration {
    uint32_t rayCount = AudiosContext::GetInstance().getRayCount();
    uint32_t maxBounce = AudiosContext::GetInstance().getMaxBounceCount();
  };

  RTEmbreeRenderingManager(Configuration conf);

  uint32_t renderScene(RTCScene render, Vector3 listenerPosition,
                       std::unordered_set<uint32_t> emitterIds,
                       AcousticRayTraceResult *resultBuffer, uint32_t rayCount);
  void rayHitTestScene(RTCScene scene);

  uint32_t getRayCount();
};

} // namespace audios