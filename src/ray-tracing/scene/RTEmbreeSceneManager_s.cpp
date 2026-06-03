#include "RTEmbreeSceneManager_p.h"
#include "_external/private_embree.h" // IWYU pragma: keep
#include "audios/file_processing.h"
#include "private_macros.h"
#include "ray-tracing/geometry/RTEmbreeGeometry_p.h"
#include <memory>

namespace audios {

RTEmbreeSceneManager::RTEmbreeSceneManager(const RTCDevice &mainDevice)
    : _mainDevice(mainDevice) {
  _pTopLevelScene.reset(rtcNewScene(mainDevice));
  // maybe add some flags to the global scene.
}

RTEmbreeSceneManager::~RTEmbreeSceneManager() = default;

int RTEmbreeSceneManager::registerGeometryFromBinaryFile(std::string filePath) {
  BinaryGeometryReader reader;
  reader.openFile(filePath);

  auto vertex_o = reader.readVertices();
  auto index_o = reader.readIndexes(reader.getIndexPairCount());
  if (!vertex_o || !index_o)
    return -1;

  std::vector<Vector3> vertices = vertex_o.value();
  std::vector<IndexTriple> indexes = index_o.value();

  USE_LOGGING("Successfully processed binary file.");

  auto pGeometryScene = std::make_unique<RTEmbreeGeometryScene>(
      _mainDevice, RTC_GEOMETRY_TYPE_TRIANGLE, std::move(vertices),
      std::move(indexes));
  _geometrySceneLibrary.push_back(std::move(pGeometryScene));

  return _geometrySceneLibrary.size() - 1;
}

int RTEmbreeSceneManager::instanceGeometryFromLibrary(
    int geometrySceneId, AffineTransformMatrix transform, int targetId) {
  if (geometrySceneId < 0 || geometrySceneId >= _geometrySceneLibrary.size()) {
    USE_LOGGING_ERROR("Can't instance geometry scene with id "
                      << geometrySceneId << " : Invalid Id");
    return -1;
  }

  RTCGeometry geom = rtcNewGeometry(_mainDevice, RTC_GEOMETRY_TYPE_INSTANCE);
  rtcSetGeometryInstancedScene(
      geom, _geometrySceneLibrary[geometrySceneId]->getEmbreeScene());
  rtcSetGeometryTransform(geom, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                          &transform);

  uint32_t geometryIdInMainScene =
      rtcAttachGeometry(_pTopLevelScene.get(), geom);

  if (targetId != -1) {
    _sceneInstances.insert({targetId, geometryIdInMainScene});
  }

  rtcReleaseGeometry(geom);

  return targetId;
}

RTCScene RTEmbreeSceneManager::getMainScene() { return _pTopLevelScene.get(); }

} // namespace audios