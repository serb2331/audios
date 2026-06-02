#pragma once

#include "export.h"

namespace audios {

typedef struct alignas(16) AUDIOS_EXPORT Vertex3 {
  float x;
  float y;
  float z;
  float pad;
} Vector3;

} // namespace audios