#include "RTEmbreeGeometryScene_p.h"
#include "private_macros.h"

namespace audios {

RTEmbreeGeometryScene::RTEmbreeGeometryScene(
    const RTCDevice &device, RTCGeometryType type,
    std::vector<Vector3> &&vertexBuffer, std::vector<IndexTriple> &&indexBuffer)
    : _geometryType(type), _vertexBuffer(std::move(vertexBuffer)),
      _indexBuffer(std::move(indexBuffer)) {
  USE_LOGGING("Creating Geometry Scene");

  _pGeometry.reset(rtcNewGeometry(device, type));
  _pInstanceScene.reset(rtcNewScene(device));

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_VERTEX, 0,
                             RTC_FORMAT_FLOAT3, _vertexBuffer.data(), 0, 16,
                             _vertexBuffer.size());
  USE_EMBREE_DEVICE_ERROR_LOG(device,
                              "Printing error after setting vertex buffer:");

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_INDEX, 0,
                             RTC_FORMAT_UINT3, _indexBuffer.data(), 0, 16,
                             _indexBuffer.size());

  USE_EMBREE_DEVICE_ERROR_LOG(device,
                              "Printing error after setting index buffer");
  rtcCommitGeometry(_pGeometry.get());

  rtcAttachGeometry(_pInstanceScene.get(), _pGeometry.get());
  rtcCommitScene(_pInstanceScene.get());

  USE_EMBREE_DEVICE_ERROR_LOG(device, "Printing error after final commits");

  USE_LOGGING("Finished creating scene")
}

RTCGeometry RTEmbreeGeometryScene::getEmbreeGeometry() {
  return _pGeometry.get();
}

RTCScene RTEmbreeGeometryScene::getEmbreeScene() {
  return _pInstanceScene.get();
}

} // namespace audios