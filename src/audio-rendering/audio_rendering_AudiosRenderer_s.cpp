#include "audio-rendering/delay-line/DelayLine_p.h"
#include "audios/audio_rendering.h"
#include "private_macros.h"
#include <algorithm>
#include <limits>
#include <memory>

namespace audios {

struct AudiosRenderer::AudiosRendererImpl {
  float speedOfSound;
  uint32_t sampleRate;
  float minDistance;
  uint32_t delayLineCount;

  std::vector<DelayLine> delayLines;

  AudiosRendererImpl(Configuration conf);

  std::vector<RayDelayLineData>
  processDelayLineDataFromResults(AcousticRayTraceResult *rayTraceResult,
                                  uint32_t resultCount);
  void updateDelayLinesWithNewClosestMatch(
      std::vector<RayDelayLineData> newDelayLines);

  uint32_t findFreeOrQuietestDelayLine();
};

uint32_t AudiosRenderer::AudiosRendererImpl::findFreeOrQuietestDelayLine() {
  uint32_t quietestIndex = -1;
  float quietestActualGain = std::numeric_limits<float>::infinity();
  for (uint32_t i = 0; i < delayLines.size(); i += 1) {
    if (!delayLines[i].isActive)
      return i;
    // We check currentGain instead of targetGain to find the voice that is 
    // actually the quietest right now, even if it's currently fading out.
    if (delayLines[i].currentGain < quietestActualGain) {
      quietestIndex = i;
      quietestActualGain = delayLines[i].currentGain;
    }
  }
  return quietestIndex;
}

AudiosRenderer::AudiosRendererImpl::AudiosRendererImpl(Configuration conf)
    : speedOfSound(conf.speedOfSound), sampleRate(conf.sampleRate),
      minDistance(conf.minDistance), delayLineCount(conf.delayLineCount),
      delayLines(conf.delayLineCount) {
  for (uint32_t i = 0; i < conf.delayLineCount; i += 1) {
    delayLines[i].delayBuffer = std::vector(conf.sampleRate * 2, 0.0f);
  }
}

std::vector<RayDelayLineData>
AudiosRenderer::AudiosRendererImpl::processDelayLineDataFromResults(
    AcousticRayTraceResult *rayTraceResult, uint32_t resultCount) {

  std::vector<RayDelayLineData> delayLineData;
  delayLineData.reserve(resultCount);

  for (size_t i = 0; i < resultCount; ++i) {
    // USE_LOGGING("Processing result #" << i);
    AcousticRayTraceResult &ray = rayTraceResult[i];

    RayDelayLineData data;

    // Calculate Delay in Samples
    data.targetDelaySamples = (ray.dist / speedOfSound) * sampleRate;

    // Calculate Gain - Air Pressure = sqrt(Intensity) (Inverse Distance Law)
    float clampedDist = std::max(ray.dist, minDistance);
    float distanceAttenuation = 1.0f / clampedDist;

    data.targetGain = distanceAttenuation * ray.en / std::sqrt(delayLineCount);

    data.directionOfArrival = ray.startDirection;

    if (data.targetDelaySamples < sampleRate * 2)
      delayLineData.push_back(data);
  }

  // Sort rays by Volume (Highest Gain first)
  std::sort(delayLineData.begin(), delayLineData.end(),
            [](const RayDelayLineData &a, const RayDelayLineData &b) {
              return a.targetGain > b.targetGain;
            });

  delayLineData.resize(delayLineCount, DISABLED_DELAY_LINE);

  return delayLineData;
}

void AudiosRenderer::AudiosRendererImpl::updateDelayLinesWithNewClosestMatch(
    std::vector<RayDelayLineData> newDelayLines) {

  for (auto &dl : delayLines) {
    dl.matchedThisFrame = false;
  }

  // Iterate through newly traced, sorted top ray paths
  for (const auto &newDelayLineData : newDelayLines) {
    int bestMatchIndex = -1;
    float bestScore = std::numeric_limits<float>::infinity();

    // Find the existing active delay line playing this specific reflection
    for (uint32_t v = 0; v < delayLineCount; ++v) {
      auto &dl = delayLines[v];

      if (dl.isActive && !dl.matchedThisFrame) {
        // USE_LOGGING("Comparinig delay line data with voice #" << v)

        // Calculate Euclidean distance between direction
        // vectors and the difference in delay times.
        float dirDiff =
            (dl.directionOfArrival - newDelayLineData.directionOfArrival)
                .length();
        float delayDiff = std::abs(dl.targetDelaySamples -
                                   newDelayLineData.targetDelaySamples);

        // If the ray is within a reasonable physical threshold (e.g., moved
        // less than 1 meter)
        if (dirDiff < 0.4f && delayDiff < 200.0f) {
          float score = dirDiff + (delayDiff * 0.001f);
          if (score < bestScore) {
            bestScore = score;
            bestMatchIndex = v;
          }
        }
        // USE_LOGGING("Scores: " << dirDiff << " " << delayDiff << " "
        //                        << dl.directionOfArrival << " "
        //                        << newDelayLineData.directionOfArrival)
      }
    }
    if (bestMatchIndex != -1) {
      // MATCH FOUND:
      // USE_LOGGING("MATCH!")
      delayLines[bestMatchIndex].update(newDelayLineData, false);
      delayLines[bestMatchIndex].matchedThisFrame = true;
    } else {
      // NO MATCH:
      // USE_LOGGING("NO MATCH!")
      int freeDl = findFreeOrQuietestDelayLine();
      delayLines[freeDl].update(newDelayLineData, true);
      delayLines[freeDl].matchedThisFrame = true;
    }
  }

  for (auto &voice : delayLines) {
    if (voice.isActive && !voice.matchedThisFrame) {
      voice.targetGain = 0.0f;
      // NO change to targetDelaySamples, so that it plays out its tail
    }
  }
}

/////////////////////

AudiosRenderer::AudiosRenderer(Configuration conf)
    : _impl(std::make_unique<AudiosRendererImpl>(conf)) {}

AudiosRenderer::~AudiosRenderer() = default;

void AudiosRenderer::processTracingResults(
    AcousticRayTraceResult *rayTraceResult, uint32_t resultCount) {
  USE_LOGGING("Processing tracing results ("
              << resultCount << ") to format DelayLines of renderer");

  auto delayLineData =
      _impl->processDelayLineDataFromResults(rayTraceResult, resultCount);

  // for (auto dld : delayLineData) {
  //   USE_LOGGING("DLD: " << dld.targetDelaySamples << " " << dld.targetGain
  //                       << " " << dld.directionOfArrival);
  // }

  _impl->updateDelayLinesWithNewClosestMatch(delayLineData);

  return;
}

void AudiosRenderer::processAudioBuffer(const float *inputBuffer,
                                        float *outputBuffer,
                                        uint32_t numFrames) {
  // 1. Clear the output buffer first (since we will be summing into it)
  for (uint32_t i = 0; i < numFrames; ++i) {
    outputBuffer[i] = 0.0f;
  }

  // Use one-pole smoothing filters to prevent zipper noise and rapid Doppler shifts
  const float gainAlpha = 0.999f;   // ~20ms time constant at 48kHz
  const float delayAlpha = 0.9999f; // ~200ms time constant for smooth pitch glide

  for (uint32_t i = 0; i < numFrames; i += 1) {
    float drySample = inputBuffer[i];
    float mixedOutput = 0.0f;

    // Process all active voices for this single sample
    for (uint32_t v = 0; v < _impl->delayLineCount; ++v) {
      DelayLine &dl = _impl->delayLines[v];

      // Optimization: Skip fully inactive lines that have completely faded out
      if (!dl.isActive && dl.currentGain < 0.00001f) {
        continue;
      }

      dl.delayBuffer[dl.writeIndex] = drySample;

      // Advance smoothed parameters using one-pole lowpass filter
      dl.currentDelaySamples = dl.currentDelaySamples * delayAlpha + dl.targetDelaySamples * (1.0f - delayAlpha);
      dl.currentGain = dl.currentGain * gainAlpha + dl.targetGain * (1.0f - gainAlpha);

      // Denormal guard for gain
      if (dl.currentGain < 1e-9f) dl.currentGain = 0.0f;

      // Turn off the delay line once it has sufficiently faded out
      if (!dl.matchedThisFrame && dl.targetGain == 0.0f && dl.currentGain == 0.0f) {
        dl.isActive = false;
      }

      int dlBufferSize = dl.delayBuffer.size();
      float readPos = dl.writeIndex - dl.currentDelaySamples;

      while (readPos < 0.0f) {
        readPos += dlBufferSize;
      }

      // 4-Point Hermite Interpolation for high-quality delay modulation
      float floorReadPos = std::floor(readPos);
      float fraction = readPos - floorReadPos;
      int i1 = static_cast<int>(floorReadPos);
      if (i1 >= dlBufferSize) i1 -= dlBufferSize;
      
      int i0 = i1 - 1;
      if (i0 < 0) i0 += dlBufferSize;
      int i2 = (i1 + 1) % dlBufferSize;
      int i3 = (i1 + 2) % dlBufferSize;

      float y0 = dl.delayBuffer[i0];
      float y1 = dl.delayBuffer[i1];
      float y2 = dl.delayBuffer[i2];
      float y3 = dl.delayBuffer[i3];

      // Hermite coefficients
      float a = (3.0f * (y1 - y2) - y0 + y3) * 0.5f;
      float b = 2.0f * y2 + y0 - (5.0f * y1 + y3) * 0.5f;
      float c = (y2 - y0) * 0.5f;
      float delayedSample = (((a * fraction) + b) * fraction + c) * fraction + y1;

      mixedOutput += delayedSample * dl.currentGain;

      dl.writeIndex = (dl.writeIndex + 1) % dlBufferSize;
    }

    // Final stage: Simple soft-clipper to prevent harsh digital distortion
    // This allows the sum of 16 delay lines to saturate gracefully if they exceed 1.0
    outputBuffer[i] = mixedOutput / std::sqrt(1.0f + mixedOutput * mixedOutput);
  }
}

} // namespace audios