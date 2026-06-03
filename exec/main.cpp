#include "audios/file_processing.h"
#include "audios/ray_tracer.h"
#include <audios.h>
#include <audios/configuration.h>
#include <iostream>
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

  //////////////////

  // RTCDevice device = rtcNewDevice("verbose=1");
  // RTCScene scene = rtcNewScene(device);
  // RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

  // float *vb = (float *)rtcSetNewGeometryBuffer(
  //     geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
  //     3);
  // vb[0] = -0.1f;
  // vb[1] = -0.1f;
  // vb[2] = 0.f; // 1st vertex
  // vb[3] = 1.f;
  // vb[4] = 0.f;
  // vb[5] = 0.f; // 2nd vertex
  // vb[6] = 0.f;
  // vb[7] = 1.f;
  // vb[8] = 0.f; // 3rd vertex

  // unsigned *ib = (unsigned *)rtcSetNewGeometryBuffer(
  //     geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned),
  //     1);
  // ib[0] = 0;
  // ib[1] = 1;
  // ib[2] = 2;

  // rtcCommitGeometry(geom);
  // rtcAttachGeometry(scene, geom);
  // rtcReleaseGeometry(geom);
  // rtcCommitScene(scene);

  // RTCRayHit rayhit;
  // rayhit.ray.org_x = 0.f;
  // rayhit.ray.org_y = 0.f;
  // rayhit.ray.org_z = -1.f;
  // rayhit.ray.dir_x = 0.f;
  // rayhit.ray.dir_y = 0.f;
  // rayhit.ray.dir_z = 1.f;
  // rayhit.ray.tnear = 0.f;
  // rayhit.ray.tfar = std::numeric_limits<float>::infinity();
  // rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

  // rayhit.ray.mask = 0xFFFFFFFF;
  // rayhit.ray.flags = 0;
  // rayhit.ray.time = 0;

  // RTCRayQueryContext context;
  // rtcInitRayQueryContext(&context);

  // rtcIntersect1(scene, &rayhit);

  // if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
  //   std::cout << "Intersection at t = " << rayhit.ray.tfar << std::endl;
  // } else {
  //   std::cout << "No Intersection" << std::endl;
  // }

  // rtcReleaseScene(scene);
  // rtcReleaseDevice(device);



  // audios::BinaryGeometryReader reader{};

  // reader.openFile("geometry/room_geometry-CSGBakedMeshInstance3D.bin");

  // std::cout << reader.getIndexCount() << " " << reader.getVertexCount() << "\n";

  // auto vertices_o = reader.readVertices();
  // auto indexes_o = reader.readIndexes(reader.getIndexCount() / 3);

  // if (!vertices_o || !indexes_o)
  //   std::cout << "\nfailed reading vertices or indexes";

  // auto vertices = vertices_o.value();
  // auto indexes = indexes_o.value();

  // std::cout << "VERTEX COUNT: " << vertices.size() << "\n";

  // std::cout << vertices[0].x << " " << vertices[0].y << " " << vertices[0].z
  //           << " " << vertices[0].pad << "\n";
  // std::cout << vertices[1].x << " " << vertices[1].y << " " << vertices[1].z
  //           << " " << vertices[0].pad << "\n";
  // std::cout << vertices[2].x << " " << vertices[2].y << " " << vertices[2].z
  //           << " " << vertices[0].pad << "\n";

  // std::cout << indexes[0].one << " " << indexes[0].two << " " << indexes[0].three << " "
  //           << indexes[0].padding << "\n";



  audios::RTEmbreeFacade facade;

  facade.registerGeometryFromBinaryFile(
      "geometry/room_geometry-CSGBakedMeshInstance3D.bin");

  return 0;
}
