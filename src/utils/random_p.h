#pragma once

#include "audios/geometry.h"
#include <random>

namespace audios {

class AudiosRandomizer {
private:
public:
  inline static bool isSetup = false;
  inline static std::uniform_real_distribution<float> ud;
  inline static std::mt19937 gen;

  static void Setup();

  static Vector3 randomSpherePoint();
};

} // namespace audios