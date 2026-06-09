#include "RTEmbreeSceneManager_p.h"
#include "_external/private_embree.h" // IWYU pragma: keep
#include "audios/file_processing.h"
#include "private_macros.h"
#include <embree4/rtcore_buffer.h>
#include <embree4/rtcore_common.h>
#include <embree4/rtcore_geometry.h>
#include <embree4/rtcore_scene.h>
#include <memory>

namespace audios {

RTEmbreeSceneManager::RTEmbreeSceneManager(const RTCDevice mainDevice)
    : _mainDevice(mainDevice) {
  _pTopLevelScene.reset(rtcNewScene(mainDevice));
  // maybe add some flags to the global scene.
}

RTEmbreeSceneManager::~RTEmbreeSceneManager() = default;

uint32_t
RTEmbreeSceneManager::registerGeometryFromBinaryFile(std::string filePath,
                                                     uint32_t registerId) {

  if (registerId != static_cast<uint32_t>(-1) &&
      _geometryLibraryIdMap.contains(registerId)) {
    USE_LOGGING_ERROR("Given register id is already occupied");
    return -1;
  }

  BinaryGeometryReader reader;
  reader.openFile(filePath);

  auto vertex_o = reader.readVertices();
  auto index_o = reader.readIndexes(reader.getIndexPairCount());
  if (!vertex_o || !index_o) {
    return -1;
  }

  std::vector<Vector3> vertices = vertex_o.value();
  std::vector<IndexTriple> indexes = index_o.value();

  USE_LOGGING("Successfully processed binary file.");

  auto pGeometryScene = std::make_unique<RTEmbreeGeometryScene>(
      _mainDevice, RTC_GEOMETRY_TYPE_TRIANGLE, std::move(vertices),
      std::move(indexes));
  _geometrySceneLibrary.push_back(std::move(pGeometryScene));

  if (registerId == static_cast<uint32_t>(-1)) {
    USE_LOGGING("Registering untracked geometry");
    return -1;
  }
  _geometryLibraryIdMap.insert({registerId, _geometrySceneLibrary.size() - 1});

  return registerId;
}

uint32_t RTEmbreeSceneManager::instanceGeometryFromLibrary(
    uint32_t geometrySceneId, AffineTransformMatrix transform,
    uint32_t registerId) {
  USE_LOGGING("Instancing geometry with id: " << geometrySceneId)
  if (!_geometryLibraryIdMap.contains(geometrySceneId)) {
    USE_LOGGING_ERROR("No geometry with registered id: " << geometrySceneId);
    return -1;
  }

  uint32_t internalGeometrySceneId = _geometryLibraryIdMap[geometrySceneId];

  if (internalGeometrySceneId >= _geometrySceneLibrary.size()) {
    USE_LOGGING_ERROR("Can't instance geometry scene with id "
                      << internalGeometrySceneId << " : Invalid Id");
    return -1;
  }

  RTCScene subScene =
      _geometrySceneLibrary[internalGeometrySceneId]->getEmbreeScene();

  RTCGeometry geom = rtcNewGeometry(_mainDevice, RTC_GEOMETRY_TYPE_INSTANCE);
  USE_EMBREE_DEVICE_ERROR(_mainDevice)
  rtcSetGeometryInstancedScene(geom, subScene);
  rtcSetGeometryTransform(geom, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                          &transform);
  rtcCommitGeometry(geom);
  USE_EMBREE_DEVICE_ERROR_LOG(_mainDevice, "Error while configuring instance");

  USE_LOGGING("Successfully instanced geometry")

  uint32_t geometryIdInMainScene =
      rtcAttachGeometry(_pTopLevelScene.get(), geom);
  rtcReleaseGeometry(geom);
  rtcCommitScene(_pTopLevelScene.get());
  USE_EMBREE_DEVICE_ERROR(_mainDevice)

  _sceneInstanceIdMap.insert({registerId, geometryIdInMainScene});

  USE_LOGGING("Registered instance with id: " << registerId);
  return registerId;
}

uint32_t
RTEmbreeSceneManager::addSoundEmitterSphere(Vector3 topLevelScenePosition,
                                            float radius, uint32_t registerId) {
  RTCGeometry emitterSphere =
      rtcNewGeometry(_mainDevice, RTC_GEOMETRY_TYPE_SPHERE_POINT);

  auto sphereIndexBuffer = static_cast<float *>(rtcSetNewGeometryBuffer(
      emitterSphere, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, 16, 1));
  sphereIndexBuffer[0] = topLevelScenePosition.x;
  sphereIndexBuffer[1] = topLevelScenePosition.y;
  sphereIndexBuffer[2] = topLevelScenePosition.z;
  sphereIndexBuffer[3] = radius;

  rtcCommitGeometry(emitterSphere);

  uint32_t emitterId = rtcAttachGeometry(_pTopLevelScene.get(), emitterSphere);
  rtcCommitScene(_pTopLevelScene.get());

  _emitterSphereExternalIdMap.insert({registerId, emitterId});
  _emitterSphereInternalIdSet.insert(emitterId);

  USE_LOGGING("Added emitter sphere with registered id: " << registerId);

  return registerId;
}

RTCScene RTEmbreeSceneManager::getMainScene() { return _pTopLevelScene.get(); }
std::unordered_set<uint32_t> RTEmbreeSceneManager::getEmitterIds() {
  return std::unordered_set(_emitterSphereInternalIdSet); // Copy
}

} // namespace audios