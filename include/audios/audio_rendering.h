#pragma once

#include "audios/export.h"
#include "audios/geometry.h"
#include "audios/ray_tracer.h"
#include <memory>

namespace audios {

constexpr float AUDIOS_SPEED_OF_SOUND = 343.0f; // m/s (1.0 in geometry is 1m)

typedef struct AUDIOS_EXPORT RayDelayLineData {
  float targetDelaySamples;
  float targetGain;
  Vector3 directionOfArrival;
  // float targetFilterCutoff; // for later when materials are implemented
} RayDelayLineData;

constexpr RayDelayLineData DISABLED_DELAY_LINE = {
    0.0f, 0.0f, {0.0f, 0.0f, 0.0f, 0.0f}};

class AUDIOS_EXPORT AudiosRenderer {
private:
  struct AudiosRendererImpl;
  std::unique_ptr<AudiosRendererImpl> _impl;

public:
  struct Configuration {
    float speedOfSound = AUDIOS_SPEED_OF_SOUND;
    uint32_t sampleRate = 48000;
    float minDistance = 1.0;
    uint32_t delayLineCount = 16;
  };

  AudiosRenderer(Configuration conf);
  ~AudiosRenderer();

  void processTracingResults(AcousticRayTraceResult *resultBuffer,
                             uint32_t resultCount, Vector3 listenerDirection);

  void processAudioBuffer(const float *inputBuffer, float *outputBuffer,
                          uint32_t numFrames);

  void processAudioBufferStereo(const float *inputBuffer, float *outputLeft,
                                float *outputRight, uint32_t numFrames);
};

} // namespace audios