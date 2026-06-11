#pragma once

#include "_external/private_embree.h"
#include "audios/geometry.h"
#include "private_macros.h"
#include "ray-tracing/geometry/RTEmbreeGeometryScene_p.h"
#include <cstdint>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace audios {

class RTEmbreeSceneManager {
private:
  const RTCDevice _mainDevice;
  std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>
      _pTopLevelScene;

  std::vector<std::unique_ptr<RTEmbreeGeometryScene>> _geometrySceneLibrary;
  // maps external id to internal library id
  std::unordered_map<uint32_t, uint32_t> _geometryLibraryIdMap;

  // maps an external id to an internal id of geometry attached to TopLevelScene
  std::unordered_map<uint32_t, uint32_t> _sceneInstanceIdMap;
  std::unordered_map<uint32_t, uint32_t> _emitterSphereExternalIdMap;
  std::unordered_set<uint32_t> _emitterSphereInternalIdSet;

public:
  RTEmbreeSceneManager(const RTCDevice mainDevice);
  ~RTEmbreeSceneManager();

  uint32_t registerGeometryFromBinaryFile(std::string filePath,
                                          uint32_t registerId);
  uint32_t registerGeometryFromBuffer(const float *vertices, uint32_t nVertices,
                                      const uint32_t *indexes,
                                      uint32_t nIndexes, uint32_t registerId);

  uint32_t instanceGeometryFromLibrary(uint32_t geometrySceneId,
                                       AffineTransformMatrix transform,
                                       uint32_t registerId);
  uint32_t addSoundEmitterSphere(Vector3 topLevelScenePosition, float radius,
                                 uint32_t registerId);

  RTCScene getMainScene();
  std::unordered_set<uint32_t> getEmitterIds();
};

} // namespace audios