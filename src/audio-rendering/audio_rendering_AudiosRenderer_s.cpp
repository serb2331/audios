#include "audio-rendering/delay-line/DelayLine_p.h"
#include "audios/audio_rendering.h"
#include "private_macros.h"
#include <algorithm>
#include <atomic>
#include <limits>
#include <memory>
#include <numbers>

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
    if (!delayLines[i].shared.isActive.load(std::memory_order_relaxed))
      return i;
    if (delayLines[i].shared.targetGain.load(std::memory_order_relaxed) <
        quietestActualGain) {
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
    dl.shared.matchedThisFrame.store(false, std::memory_order_relaxed);
  }

  // Iterate through newly traced, sorted top ray paths
  for (const auto &newDelayLineData : newDelayLines) {

    if (newDelayLineData.targetGain <= 0.0f)
      continue;

    int bestMatchIndex = -1;
    float bestScore = std::numeric_limits<float>::infinity();

    // Find the existing active delay line playing this specific reflection
    for (uint32_t v = 0; v < delayLineCount; ++v) {
      auto &dl = delayLines[v];

      if (!dl.shared.isActive.load(std::memory_order_relaxed))
        continue;
      if (dl.shared.matchedThisFrame.load(std::memory_order_relaxed))
        continue;

      // USE_LOGGING("Comparinig delay line data with voice #" << v)

      // Calculate Euclidean distance between direction
      // vectors and the difference in delay times.
      float dirDiff =
          (dl.shared.directionOfArrival - newDelayLineData.directionOfArrival)
              .length();
      float delayDiff = std::abs(
          dl.shared.targetDelaySamples.load(std::memory_order_relaxed) -
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
    if (bestMatchIndex != -1) {
      // MATCH FOUND:
      // USE_LOGGING("MATCH!")
      delayLines[bestMatchIndex].update(newDelayLineData, false);
      delayLines[bestMatchIndex].shared.matchedThisFrame.store(
          true, std::memory_order_relaxed);
    } else {
      // NO MATCH:
      // USE_LOGGING("NO MATCH!")
      int freeDl = findFreeOrQuietestDelayLine();
      delayLines[freeDl].update(newDelayLineData, true);
      delayLines[freeDl].shared.matchedThisFrame.store(
          true, std::memory_order_relaxed);
    }
  }

  // Any active line not matched this frame has no corresponding ray path —
  // fade it out by zeroing its target gain. Do NOT set isActive = false here;
  // let the audio thread deactivate it naturally once currentGain reaches 0.
  for (auto &voice : delayLines) {
    if (voice.shared.isActive.load(std::memory_order_relaxed) &&
        !voice.shared.matchedThisFrame.load(std::memory_order_relaxed)) {
      voice.shared.targetGain.store(0.0f, std::memory_order_relaxed);
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

  // Use one-pole smoothing filters to prevent zipper noise and rapid Doppler
  // shifts
  const float gainAlpha = 0.999f; // ~20ms time constant at 48kHz
  const float delayAlpha =
      0.9999f; // ~200ms time constant for smooth pitch glide

  for (uint32_t i = 0; i < numFrames; i += 1) {
    float drySample = inputBuffer[i];
    float mixedOutput = 0.0f;

    // Process all active voices for this single sample
    for (uint32_t v = 0; v < _impl->delayLineCount; ++v) {
      DelayLine &dl = _impl->delayLines[v];

      if (!dl.shared.isActive.load(std::memory_order_acquire))
        continue;

      int dlBufferSize = static_cast<int>(dl.delayBuffer.size());

      dl.delayBuffer[dl.writeIndex] = drySample;
      dl.writeIndex =
          (dl.writeIndex + 1) % dlBufferSize; // advance unconditionally

      float targetDelay =
          dl.shared.targetDelaySamples.load(std::memory_order_relaxed);
      float targetGain = dl.shared.targetGain.load(std::memory_order_relaxed);

      bool isNewPath =
          dl.shared.isNewPath.exchange(false, std::memory_order_acq_rel);

      if (isNewPath) {
        dl.currentDelaySamples = targetDelay; // snap delay
        dl.currentGain = 0.0f;                // fade in from silence
      } // Investigate if the snapping followed immediately by smoothing is
        // affecting the sound

      // then smooth toward targetDelay / targetGain as before
      dl.currentDelaySamples = dl.currentDelaySamples * delayAlpha +
                               targetDelay * (1.0f - delayAlpha);
      dl.currentGain =
          dl.currentGain * gainAlpha + targetGain * (1.0f - gainAlpha);

      // Optimization: Skip fully inactive lines that have completely faded out
      // Denormal guard for gain
      if (dl.currentGain < 0.00001f) {
        if (dl.currentGain < 1e-9f) {
          dl.currentGain = 0.0f;

          if (targetGain == 0.0f) {
            dl.shared.isActive.store(false, std::memory_order_release);
          }
        }
        continue;
      }

      float readPos = dl.writeIndex - dl.currentDelaySamples;

      while (readPos < 0.0f)
        readPos += dlBufferSize;

      // 4-Point Hermite Interpolation for high-quality delay modulation
      float floorReadPos = std::floor(readPos);
      float fraction = readPos - floorReadPos;
      int i1 = static_cast<int>(floorReadPos);
      if (i1 >= dlBufferSize)
        i1 -= dlBufferSize;

      int i0 = i1 - 1;
      if (i0 < 0)
        i0 += dlBufferSize;
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
      float delayedSample =
          (((a * fraction) + b) * fraction + c) * fraction + y1;

      mixedOutput += delayedSample * dl.currentGain;
    }

    // Final stage: Simple soft-clipper to prevent harsh digital distortion
    // This allows the sum of 16 delay lines to saturate gracefully if they
    // exceed 1.0
    outputBuffer[i] = mixedOutput / std::sqrt(1.0f + mixedOutput * mixedOutput);
  }
}

void AudiosRenderer::processAudioBufferStereo(const float *inputBuffer,
                                              float *outputLeft,
                                              float *outputRight,
                                              uint32_t numFrames) {
  // Zero output
  std::fill(outputLeft, outputLeft + numFrames, 0.0f);
  std::fill(outputRight, outputRight + numFrames, 0.0f);

  const float gainAlpha = 0.999f;
  const float delayAlpha = 0.9999f;

  for (uint32_t i = 0; i < numFrames; ++i) {
    float drySample = inputBuffer[i];

    for (uint32_t v = 0; v < _impl->delayLineCount; ++v) {
      DelayLine &dl = _impl->delayLines[v];

      if (!dl.shared.isActive.load(std::memory_order_acquire))
        continue;

      int dlBufferSize = static_cast<int>(dl.delayBuffer.size());
      dl.delayBuffer[dl.writeIndex] = drySample;
      dl.writeIndex = (dl.writeIndex + 1) % dlBufferSize;

      float targetDelay =
          dl.shared.targetDelaySamples.load(std::memory_order_relaxed);
      float targetGain = dl.shared.targetGain.load(std::memory_order_relaxed);

      bool isNewPath =
          dl.shared.isNewPath.exchange(false, std::memory_order_acq_rel);
      if (isNewPath) {
        dl.currentDelaySamples = targetDelay;
        dl.currentGain = 0.0f;
      }

      dl.currentDelaySamples = dl.currentDelaySamples * delayAlpha +
                               targetDelay * (1.0f - delayAlpha);
      dl.currentGain =
          dl.currentGain * gainAlpha + targetGain * (1.0f - gainAlpha);

      if (dl.currentGain < 0.00001f) {
        if (dl.currentGain < 1e-9f) {
          dl.currentGain = 0.0f;

          if (targetGain == 0.0f) {
            dl.shared.isActive.store(false, std::memory_order_release);
          }
        }
        continue;
      }

      // --- Hermite interpolation ---
      float readPos = dl.writeIndex - dl.currentDelaySamples;
      while (readPos < 0.0f)
        readPos += dlBufferSize;

      float floorReadPos = std::floor(readPos);
      float fraction = readPos - floorReadPos;
      int i1 = static_cast<int>(floorReadPos) % dlBufferSize;
      int i0 = (i1 - 1 + dlBufferSize) % dlBufferSize;
      int i2 = (i1 + 1) % dlBufferSize;
      int i3 = (i1 + 2) % dlBufferSize;

      float y0 = dl.delayBuffer[i0], y1 = dl.delayBuffer[i1];
      float y2 = dl.delayBuffer[i2], y3 = dl.delayBuffer[i3];

      float a = (3.0f * (y1 - y2) - y0 + y3) * 0.5f;
      float b = 2.0f * y2 + y0 - (5.0f * y1 + y3) * 0.5f;
      float c = (y2 - y0) * 0.5f;
      float delayedSample =
          (((a * fraction) + b) * fraction + c) * fraction + y1;

      // ── Constant-power pan from directionOfArrival ────────────────
      // directionOfArrival.x is left/right in listener space (-1..+1).
      float panX = std::clamp(dl.shared.directionOfArrival.x, -1.0f, 1.0f);
      float panAngle =
          (panX + 1.0f) * 0.5f * (std::numbers::pi * 0.5f); // 0..π/2
      float panL = std::cos(panAngle);
      float panR = std::sin(panAngle);

      float gainedSample = delayedSample * dl.currentGain;
      outputLeft[i] += gainedSample * panL;
      outputRight[i] += gainedSample * panR;
    }

    // Soft clip per channel independently
    outputLeft[i] =
        outputLeft[i] / std::sqrt(1.0f + outputLeft[i] * outputLeft[i]);
    outputRight[i] =
        outputRight[i] / std::sqrt(1.0f + outputRight[i] * outputRight[i]);
  }
}

} // namespace audios