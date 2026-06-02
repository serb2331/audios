#include "RTEmbreeGeometry_p.h"
#include <embree4/rtcore_geometry.h>
#include <embree4/rtcore_scene.h>

namespace audios {

RTEmbreeGeometryScene::RTEmbreeGeometryScene(
    const RTCDevice &device, RTCGeometryType type,
    std::vector<Vector3> &&vertexBuffer, std::vector<uint32_t> &&indexBuffer)
    : _geometryType(type), _vertexBuffer(vertexBuffer),
      _indexBuffer(indexBuffer) {
  USE_LOGGING("Creating Geometry Scene");

  _pGeometry.reset(rtcNewGeometry(device, type));
  _pInstanceScene.reset(rtcNewScene(device));

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_VERTEX, 0,
                             RTC_FORMAT_FLOAT3, _vertexBuffer.data(), 0, 16,
                             _vertexBuffer.size());
  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_INDEX, 1,
                             RTC_FORMAT_UINT3, _indexBuffer.data(), 0, 16,
                             _indexBuffer.size());
  rtcCommitGeometry(_pGeometry.get());

  rtcAttachGeometry(_pInstanceScene.get(), _pGeometry.get());
  rtcCommitScene(_pInstanceScene.get());
}

RTCGeometry RTEmbreeGeometryScene::getEmbreeGeometry() {
  return _pGeometry.get();
}

} // namespace audios