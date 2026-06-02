#pragma once

#include "_external/private_embree.h"
#include "audios/geometry.h"
#include "private_macros.h"
#include <memory>
#include <type_traits>
#include <vector>

namespace audios {

class RTEmbreeGeometryScene {
private:
  std::unique_ptr<std::remove_pointer_t<RTCGeometry>, RTCGeometryDeleter>
      _pGeometry;
  std::unique_ptr<std::remove_pointer_t<RTCScene>, RTCSceneDeleter>
      _pInstanceScene;
  RTCGeometryType _geometryType;
  std::vector<Vector3> _vertexBuffer;
  std::vector<uint32_t> _indexBuffer;

public:
  RTEmbreeGeometryScene(const RTCDevice &device, RTCGeometryType type,
                        std::vector<Vector3> &&vertexBuffer,
                        std::vector<uint32_t> &&indexBuffer);
  RTEmbreeGeometryScene(const RTEmbreeGeometryScene &) = delete;
  RTEmbreeGeometryScene(RTEmbreeGeometryScene &&) noexcept = default;

  RTEmbreeGeometryScene &operator=(const RTEmbreeGeometryScene &) = delete;
  RTEmbreeGeometryScene &operator=(RTEmbreeGeometryScene &&) noexcept = default;

  RTCGeometry getEmbreeGeometry();
};

} // namespace audios