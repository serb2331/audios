#include "RTEmbreeSceneManager_p.h"
#include "_external/private_embree.h" // IWYU pragma: keep
#include "audios/file_processing.h"
#include "private_macros.h"
#include "ray-tracing/geometry/RTEmbreeGeometry_p.h"
#include <embree4/rtcore_buffer.h>
#include <embree4/rtcore_common.h>
#include <embree4/rtcore_device.h>
#include <embree4/rtcore_geometry.h>
#include <embree4/rtcore_scene.h>
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
  std::vector<uint32_t> indexes = index_o.value();

  USE_LOGGING("Successfully processed binary file.");

  auto pGeometryScene = std::make_unique<RTEmbreeGeometryScene>(
      _mainDevice, RTC_GEOMETRY_TYPE_TRIANGLE, std::move(vertices),
      std::move(indexes));
  _geometrySceneLibrary.push_back(std::move(pGeometryScene));

  return _geometrySceneLibrary.size() - 1;
}

} // namespace audios