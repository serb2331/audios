#include "DelayLine_p.h"

namespace audios {

void DelayLine::update(RayDelayLineData info, bool isNewPath) {
  shared.targetDelaySamples.store(info.targetDelaySamples,
                                  std::memory_order_relaxed);
  shared.targetGain.store(info.targetGain, std::memory_order_relaxed);
  shared.directionOfArrival =
      info.directionOfArrival; // Vector3 write, audio thread only reads
  shared.isNewPath.store(isNewPath, std::memory_order_release);
  shared.isActive.store(true, std::memory_order_release);
}

// void DelayLine::update(RayDelayLineData delayLineInformation, bool isNewPath)
// {
//   if (isNewPath || !isActive) {
//     // SNAP the delay instantly to avoid the pitch-warp click.
//     targetDelaySamples = delayLineInformation.targetDelaySamples;
//     currentDelaySamples = delayLineInformation.targetDelaySamples;

//     // Fade the volume IN from 0 to avoid amplitude clicks.
//     targetGain = delayLineInformation.targetGain;
//     currentGain = 0.0f;

//     directionOfArrival = delayLineInformation.directionOfArrival;
//     isActive = true;
//   } else {
//     // Smoothly interpolate the delay and gain (the player/source moved
//     // slightly)
//     targetDelaySamples = delayLineInformation.targetDelaySamples;
//     targetGain = delayLineInformation.targetGain;
//     directionOfArrival = delayLineInformation.directionOfArrival;
//   }
// }

} // namespace audios