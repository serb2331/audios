#pragma once

#include "audios/audio_rendering.h"
#include "audios/geometry.h"
#include <vector>

namespace audios
{

struct DelayLine {
  std::vector<float> delayBuffer;
  int writeIndex = 0;
  bool isActive = false;
  bool matchedThisFrame = false;

  // Smoothing variables
  float currentDelaySamples = 0.0f;
  float currentGain = 0.0f;

  // Targets from Embree
  float targetDelaySamples = 0.0f;
  float targetGain = 0.0f;

  Vector3 directionOfArrival;
  // float targetFilterCutoff; // for later when materials are implemented

  void update(RayDelayLineData delayLineInformation, bool isNewPath);
};

}