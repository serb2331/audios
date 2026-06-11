#pragma once

#include "audios/audio_rendering.h"
#include "audios/geometry.h"
#include <atomic>
#include <vector>

namespace audios {

struct alignas(64)
    DelayLineTargets { // cacheline-aligned to prevent false sharing
  std::atomic<float> targetDelaySamples{0.0f};
  std::atomic<float> targetGain{0.0f};
  // pad to 64 bytes so audio-thread fields land on a different cacheline
  Vector3 directionOfArrival{0.0f, 0.0f, 0.0f, 0.0f};
  std::atomic<bool> isNewPath{false};
  std::atomic<bool> isActive{false};

  std::atomic<bool> matchedThisFrame{false};
};

struct DelayLine {
  // ── Audio thread owns these ──────────────────────────────────────────
  std::vector<float> delayBuffer;
  int writeIndex = 0;
  float currentDelaySamples = 0.0f;
  float currentGain = 0.0f;

  // ── Shared: game thread writes, audio thread reads ───────────────────
  // Wrapped in a struct so they land on one cacheline together
  DelayLineTargets shared;

  void update(RayDelayLineData info, bool isNew);
};

// struct DelayLine {
//   std::vector<float> delayBuffer;
//   int writeIndex = 0;
//   bool isActive = false;
//   bool matchedThisFrame = false;

//   // Smoothing variables
//   float currentDelaySamples = 0.0f;
//   float currentGain = 0.0f;

//   // Targets from Embree
//   float targetDelaySamples = 0.0f;
//   float targetGain = 0.0f;

//   Vector3 directionOfArrival;
//   // float targetFilterCutoff; // for later when materials are implemented

//   void update(RayDelayLineData delayLineInformation, bool isNewPath);
// };

} // namespace audios