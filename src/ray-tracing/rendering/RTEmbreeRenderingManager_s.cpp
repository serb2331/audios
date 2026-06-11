#include "RTEmbreeRenderingManager_p.h"
#include "private_macros.h"
#include "utils/random_p.h"
#include <limits>
#include <sys/types.h>

namespace audios {

auto getTopLevelHitId = [](const RTCRayHit &rh) -> uint32_t {
  if (rh.hit.instID[0] != RTC_INVALID_GEOMETRY_ID) {
    return rh.hit.instID[0]; // instanced geometry — use instance id
  }
  return rh.hit.geomID; // direct geometry — geomID is top-level
};

void RTEmbreeRenderingManager::_setupRayForTrace(RTCRayHit &rh,
                                                 Vector3 newStart,
                                                 Vector3 newDir) {
  rh.ray.org_x = newStart.x;
  rh.ray.org_y = newStart.y;
  rh.ray.org_z = newStart.z;

  rh.ray.dir_x = newDir.x;
  rh.ray.dir_y = newDir.y;
  rh.ray.dir_z = newDir.z;

  rh.ray.tnear = 0.001f;
  rh.ray.tfar = std::numeric_limits<float>::infinity();

  rh.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rh.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

RTCRayHit RTEmbreeRenderingManager::_createInitialRayHit(Vector3 position,
                                                         Vector3 direction) {
  RTCRayHit rh;
  _setupRayForTrace(rh, position, direction);

  rh.ray.time = 0.0f;
  rh.ray.mask = -1;
  rh.ray.flags = 0;

  return rh;
}

RTEmbreeRenderingManager::AudioRayContext
RTEmbreeRenderingManager::_initRayContext(float rayFrequency,
                                          float initialEnergy) {
  AudioRayContext c;
  rtcInitRayQueryContext(&c.rayQueryContext);
  c.dist = 0.0f;
  c.en = initialEnergy;
  c.freq = rayFrequency;
  c.bounceCount = 0;
  return c;
}

////////

RTEmbreeRenderingManager::RTEmbreeRenderingManager(Configuration conf)
    : _pRayQueryContext(std::make_unique<RTCRayQueryContext>()),
      _pIntersectArguments(std::make_unique<RTCIntersectArguments>()),
      _rayCount(conf.rayCount), _maxBounce(conf.maxBounce) {}

void RTEmbreeRenderingManager::rayHitTestScene(RTCScene scene) {
  USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));
  auto testRayHit =
      _createInitialRayHit({0.01, 0.5, 0.0, 0}, {1.0, 0.0, 0.0, 0});

  USE_LOGGING("Formatted RayHit struct");
  USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));

  RTCRayQueryContext cont;
  rtcInitRayQueryContext(&cont);
  RTCIntersectArguments iargs;
  rtcInitIntersectArguments(&iargs);
  iargs.context = &cont;

  USE_LOGGING("Formatted Context and IntersectArgs");
  USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));

  rtcIntersect1(scene, &testRayHit, &iargs);

  USE_LOGGING("Finished tracing ray");
  USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));

  if (testRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    USE_LOGGING("Hit valid geometry!: " << testRayHit.hit.geomID);

    USE_LOGGING("rayData ");
    USE_LOGGING("distance " << testRayHit.ray.tfar);
    USE_LOGGING("instid: " << testRayHit.hit.instID[0]);
    USE_LOGGING("geomid: " << testRayHit.hit.geomID);
    USE_LOGGING("u: " << testRayHit.hit.u);
    USE_LOGGING("v: " << testRayHit.hit.v);
    USE_LOGGING("time: " << testRayHit.ray.time);

    return;
  }
  USE_LOGGING_ERROR("No valid geometry hit!: " << testRayHit.hit.geomID);
}

uint32_t
RTEmbreeRenderingManager::renderScene(RTCScene scene, Vector3 listenerPosition,
                                      std::unordered_set<uint32_t> emitterIds,
                                      AcousticRayTraceResult *resultBuffer,
                                      uint32_t rayCount) {
  USE_LOGGING("Rendering scene with " << rayCount << " rays");

  uint32_t validRayCount = 0;

  for (uint32_t i = 0; i < rayCount; i += 1) {
    Vector3 rhOrigin = listenerPosition;
    Vector3 rhDirection = AudiosRandomizer::randomSpherePoint().normalize();
    bool rayArrived = false;
    AcousticRayTraceResult res;
    res.startDirection = rhDirection;

    RTCRayHit rh = _createInitialRayHit(rhOrigin, rhDirection);
    AudioRayContext c = _initRayContext(1.0, 1.0);

    RTCIntersectArguments args;
    rtcInitIntersectArguments(&args);
    args.context = reinterpret_cast<RTCRayQueryContext *>(&c);
    // USE_LOGGING("Prepared ray trace #" << i);

    rtcIntersect1(scene, &rh, &args);

    // USE_LOGGING("Performed trace #" << c.bounceCount << ": " << rhOrigin << "
    // "
    //                                 << rhDirection);
    USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));

    while (!rayArrived && c.bounceCount <= _maxBounce &&
           rh.hit.geomID != RTC_INVALID_GEOMETRY_ID) {

      c.dist += rh.ray.tfar;

      Vector3 normal(rh.hit.Ng_x, rh.hit.Ng_y, rh.hit.Ng_z, 0.0f);
      normal = normal.normalize();

      uint32_t hitId = getTopLevelHitId(rh);

      if (emitterIds.contains(hitId)) {
        // USE_LOGGING("Ray hit emitter sphere with id: "
        //             << hitId << " after " << c.bounceCount << " bounces");
        rayArrived = true;
        res = {c.dist,
               c.en,
               c.freq,
               c.bounceCount,
               normal,
               res.startDirection,
               rhDirection.normalize(),
               hitId};
        continue;
      }

      {
        // calculate further acoustic changes
      }

      c.bounceCount++;

      rhOrigin = rhOrigin + (rhDirection * rh.ray.tfar);
      auto rhNewDir = rhDirection - normal * (rhDirection * normal) * 2;
      rhDirection = rhNewDir.normalize();

      _setupRayForTrace(rh, rhOrigin, rhDirection);

      rtcIntersect1(scene, &rh, &args);
      // USE_LOGGING("Performed trace #" << c.bounceCount << ": " << rhOrigin
      //                                 << " " << rhDirection);
      USE_EMBREE_DEVICE_ERROR(rtcGetSceneDevice(scene));
    }

    if (rayArrived) {
      // USE_LOGGING(res.dist << " " << res.bounceCount << " "
      //                      << res.finalDirection << " " << res.startDirection
      //                      << " " << res.hitEmitterId << " " <<
      //                      res.lastNormal)
      resultBuffer[validRayCount++] = res;
    }
  }

  // USE_LOGGING("Finished rendering scene with " << validRayCount
  //                                              << " arriving rays");

  return validRayCount;
}

uint32_t RTEmbreeRenderingManager::getRayCount() { return _rayCount; }

} // namespace audios