#include "audios/geometry.h"
#include <iostream>

namespace audios {

std::ostream &operator<<(std::ostream &os, const Vector3 &vector) {
  os << "{" << vector.x << " " << vector.y << " " << vector.z << "}";
  return os;
}

} // namespace audios