#pragma once

#include "_external/private_embree.h"
#include "private_macros.h"
#include "ray-tracing/geometry/RTEmbreeGeometry_p.h"
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace audios {

class RTEmbreeSceneManager {
private:
  const RTCDevice &_mainDevice;
  std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>
      _pTopLevelScene;

  // holds a mapping for each unique geomety registered
  std::vector<std::unique_ptr<RTEmbreeGeometryScene>> _geometrySceneLibrary;

  // maps an instance id from the engine to the geometry id
  // in the top level scene
  std::unordered_map<uint32_t, uint32_t> _sceneInstances;
  // ^ probably will change

public:
  RTEmbreeSceneManager(const RTCDevice &mainDevice);
  ~RTEmbreeSceneManager();

  int registerGeometryFromBinaryFile(std::string filePath);
  int instanceGeometryFromLibrary(int geometrySceneId,
                                  AffineTransformMatrix transform,
                                  int targetId = -1);

  RTCScene getMainScene();
};

} // namespace audios