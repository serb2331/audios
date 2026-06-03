#pragma once

#include "export.h"
#include <cstdint>

namespace audios {

typedef struct alignas(16) AUDIOS_EXPORT Vector3 {
  float x;
  float y;
  float z;
  float pad; // must always be 0 for proper creation
} Vector3;

typedef struct alignas(16) AUDIOS_EXPORT IndexTriple {
  uint32_t one;
  uint32_t two;
  uint32_t three;
  uint32_t padding; // must always be 0 for proper creation
} IndexTriple;

typedef struct alignas(64) AUDIOS_EXPORT AffineTransformMatrix {
  Vector3 col1;
  Vector3 col2;
  Vector3 col3;
  Vector3 translation; // padding of translation must be 1
} AffineTransformMatrix;

} // namespace audios