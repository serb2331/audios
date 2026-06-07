#include <audios.h>
#include <audios/configuration.h>
#include <stdio.h>
#include <sys/types.h>

#define PROJECT_NAME "audio-library"

static void setupProject() {
  audios::Configuration::setLogging(true);
  audios::Configuration::setBufferFrameCount(512);
}

// void error_handler(void *userPtr, RTCError code, const char *str) {
//   std::cerr << "Embree Error: " << str << std::endl;
// }

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("%s takes no arguments.\n", argv[0]);
    return 1;
  }
  printf("\n\n\nThis is project %s.\n", PROJECT_NAME);

  setupProject();

  audios::RTEmbreeFacade facade("", {10000, 25});

  uint32_t registeredId = facade.registerGeometryFromBinaryFile(
      "geometry/room_geometry-CSGBakedMeshInstance3D.bin", 0);

  facade.instanceGeometryById(
      registeredId, {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}},
      0);
  facade.addSoundEmitterSphere({4.5f, 0.5f, 4.0f, 0.0f}, 0.2f, 3);

  // facade.testMainSceneRayTrace();
  facade.renderMainScene({0.5, 0.5, 0, 0});

  return 0;
}
