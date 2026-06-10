#pragma once

#include "export.h"
#include <cmath>
#include <cstdint>
#include <ostream>

namespace audios {

typedef struct alignas(16) AUDIOS_EXPORT Vector3 {
  float x;
  float y;
  float z;
  float pad; // must always be 0 for proper creation

  Vector3 operator+(const Vector3 &other) const {
    return {x + other.x, y + other.y, z + other.z, pad};
  }

  Vector3 operator-(const Vector3 &other) const {
    return {x - other.x, y - other.y, z - other.z, pad};
  }

  float operator*(const Vector3 &other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  Vector3 operator*(const float &scale) const {
    return {x * scale, y * scale, z * scale, pad};
  }

  Vector3 operator/(const float &scale) const {
    return scale == 0 ? Vector3{0, 0, 0, 0}
                      : Vector3{x / scale, y / scale, z / scale, pad};
  }

  Vector3 normalize() const {
    float d = this->length();
    return {x / d, y / d, z / d, pad};
  }

  float length() const { return std::sqrt(x * x + y * y + z * z); }

  Vector3 toPolar() const
  {
    Vector3 polar;

    // Calculate the distance from the origin
    polar.x = std::sqrt(x * x + y * y + z * z);

    // Handle the edge case where the vector is exactly at the origin
    if (polar.x == 0.0f) {
      polar.y = 0.0f;
      polar.z = 0.0f;
      return polar;
    }

    // Calculate Azimuth (angle in XZ plane).
    // atan2(z, x) makes +x the 0-angle (front).
    polar.y = std::atan2(z, x);

    // Calculate Elevation (angle from XZ plane).
    // We use the horizontal distance to ensure numerical stability.
    float horizontalDistance = std::sqrt(x * x + z * z);
    polar.z = std::atan2(y, horizontalDistance);

    return polar;
  }

} Vector3;

std::ostream &operator<<(std::ostream &os, const Vector3 &vector);

typedef struct alignas(16) AUDIOS_EXPORT IndexTriple {
  uint32_t one;
  uint32_t two;
  uint32_t three;
  uint32_t padding; // must always be 0 for proper creation
} IndexTriple;

typedef struct alignas(16) AUDIOS_EXPORT AffineTransformMatrix {
  Vector3 col1;
  Vector3 col2;
  Vector3 col3;
  Vector3 translation; // padding of translation must be 1
} AffineTransformMatrix;

} // namespace audios