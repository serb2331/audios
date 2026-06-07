#include "random_p.h"

namespace audios {

void AudiosRandomizer::Setup() {
  if (AudiosRandomizer::isSetup)
    return;
  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  gen = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
  isSetup = true;
  ud = std::uniform_real_distribution<float>(-1.0, 1.0);
}

Vector3 AudiosRandomizer::randomSpherePoint() {
  float x1, x2, r;

  do {
    x1 = ud(gen);
    x2 = ud(gen);
    r = std::pow(x1, 2) + std::pow(x2, 2);

  } while (r >= 1.0f);

  Vector3 ret{0, 0, 0, 0};
  ret.x = 2 * x1 * std::sqrt(1 - r);
  ret.y = 2 * x2 * std::sqrt(1 - r);
  ret.z = 1 - 2 * r;

  return ret;
}

} // namespace audios
