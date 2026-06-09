#include "DelayLine_p.h"

namespace audios {

void DelayLine::update(RayDelayLineData delayLineInformation, bool isNewPath) {
  if (isNewPath || !isActive) {
    // SNAP the delay instantly to avoid the pitch-warp click.
    targetDelaySamples = delayLineInformation.targetDelaySamples;
    currentDelaySamples = delayLineInformation.targetDelaySamples;

    // Fade the volume IN from 0 to avoid amplitude clicks.
    targetGain = delayLineInformation.targetGain;
    currentGain = 0.0f;

    directionOfArrival = delayLineInformation.directionOfArrival;
    isActive = true;
  } else {
    // Smoothly interpolate the delay and gain (the player/source moved
    // slightly)
    targetDelaySamples = delayLineInformation.targetDelaySamples;
    targetGain = delayLineInformation.targetGain;
    directionOfArrival = delayLineInformation.directionOfArrival;
  }
}

} // namespace audios