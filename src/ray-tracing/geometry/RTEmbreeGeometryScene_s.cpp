#include "RTEmbreeGeometryScene_p.h"
#include "private_macros.h"

namespace audios {

RTEmbreeGeometryScene::RTEmbreeGeometryScene(
    const RTCDevice &device, RTCGeometryType type,
    std::vector<Vector3> &&vertexBuffer, std::vector<IndexTriple> &&indexBuffer)
    : _geometryType(type), _vertexBuffer(vertexBuffer),
      _indexBuffer(indexBuffer) {
  USE_LOGGING("Creating Geometry Scene");

  _pGeometry.reset(rtcNewGeometry(device, type));
  _pInstanceScene.reset(rtcNewScene(device));

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_VERTEX, 0,
                             RTC_FORMAT_FLOAT3, vertexBuffer.data(), 0, 16,
                             vertexBuffer.size());
  USE_EMBREE_DEVICE_ERROR_LOG(device,
                              "Printing error after setting vertex buffer:");

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_INDEX, 0,
                             RTC_FORMAT_UINT3, indexBuffer.data(), 0, 16,
                             indexBuffer.size());

  USE_EMBREE_DEVICE_ERROR_LOG(device,
                              "Printing error after setting index buffer");
  rtcCommitGeometry(_pGeometry.get());

  rtcAttachGeometry(_pInstanceScene.get(), _pGeometry.get());
  rtcCommitScene(_pInstanceScene.get());

  USE_EMBREE_DEVICE_ERROR_LOG(device, "Printing error after final commits");

  USE_LOGGING("Finished creating scene")
}

RTEmbreeGeometryScene::RTEmbreeGeometryScene(
    const RTCDevice &device, RTCGeometryType type, const float *vertexBuffer,
    uint32_t vertexCount, const uint32_t *indexBuffer, uint32_t indexCount)
    : _geometryType(type) {

  USE_LOGGING("Creating Geometry Scene");

  _pGeometry.reset(rtcNewGeometry(device, type));
  _pInstanceScene.reset(rtcNewScene(device));

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_VERTEX, 0,
                             RTC_FORMAT_FLOAT3, vertexBuffer, 0, 12,
                             vertexCount);
  USE_EMBREE_DEVICE_ERROR_LOG(device,
                              "Printing error after setting vertex buffer:");

  rtcSetSharedGeometryBuffer(_pGeometry.get(), RTC_BUFFER_TYPE_INDEX, 0,
                             RTC_FORMAT_UINT3, indexBuffer, 0, 12, indexCount);

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