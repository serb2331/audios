#include "audios/geometry.h"
#include <audios.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <vector>

#define PROJECT_NAME "audio-library"

static void setupProject() {
  audios::Configuration::setLogging(true);
  audios::Configuration::setBufferFrameCount(512);
}

void interleaveStereo(const float *const pLeft, const float *const pRight,
                      float *const pInterleaved, std::size_t totalFrames) {
  // Basic safety check
  if (!pLeft || !pRight || !pInterleaved) {
    return;
  }

  for (std::size_t i = 0; i < totalFrames; ++i) {
    pInterleaved[2 * i] = pLeft[i];      // Even index: Left channel
    pInterleaved[2 * i + 1] = pRight[i]; // Odd index: Right channel
  }
}

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("%s takes no arguments.\n", argv[0]);
    return 1;
  }
  printf("\n\n\nThis is project %s.\n", PROJECT_NAME);

  setupProject();

  // setup WAV file management

  audios::WAVAudioFileDecoder dec;
  dec.initFile("sounds/full-track.wav");
  auto encFormat = dec.getDataFormat();
  encFormat.channels = 2;
  uint32_t channels = encFormat.channels;

  std::vector<float> wavReadBuffer(512 * dec.getDataFormat().channels);
  std::vector<float> wavWriteBufferL(512 * (channels / 2));
  std::vector<float> wavWriteBufferR(512 * (channels / 2));
  std::vector<float> wavWriteBufferInterleaved(512 * channels);

  audios::WAVAudioFileEncoder enc;
  enc.initFile("sounds/writtenWAV-track2.wav", &encFormat);

  // setup rendering geometry

  const uint32_t rayCount = 1000;
  const uint32_t bounceCount = 25;
  audios::RTEmbreeFacade facade("", {rayCount, bounceCount});

  uint32_t registeredId = facade.registerGeometryFromBinaryFile(
      "geometry/room_geometry-CSGBakedMeshInstance3D.bin", 0);

  facade.instanceGeometryById(
      registeredId, {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}},
      0);
  facade.addSoundEmitterSphere({4.5f, 0.5f, 4.0f, 0.0f}, 0.1f, 3);

  audios::AudiosRenderer renderer({.sampleRate = encFormat.sampleRate});
  audios::Vector3 listenerPosition = {0.5, 0.5, 0.0, 0};
  audios::Vector3 listenerRotation = audios::Vector3{-1.0, 0.0, 0.0, 0.0};

  ///

  std::vector<audios::AcousticRayTraceResult> rayResults(rayCount);
  uint32_t resultCount =
      facade.renderMainScene(listenerPosition, rayResults.data(), rayCount);
  renderer.processTracingResults(rayResults.data(), resultCount,
                                 listenerRotation);

  int i = 1;
  uint32_t nrFramesRead;
  while ((nrFramesRead = dec.readFrames(wavReadBuffer.data(), 512)) == 512) {
    std::cout << "Number of read frames: " << i * 512 << " " << nrFramesRead
              << "\n";
    if (i % 6 == 0) {
      listenerPosition =
          listenerPosition + audios::Vector3{0.01, 0.0, 0.0, 0.0};
      resultCount =
          facade.renderMainScene(listenerPosition, rayResults.data(), rayCount);
      renderer.processTracingResults(rayResults.data(), resultCount,
                                     listenerRotation);
    }
    renderer.processAudioBufferStereo(wavReadBuffer.data(),
                                      wavWriteBufferL.data(),
                                      wavWriteBufferR.data(), nrFramesRead);

    // interleave L and R
    interleaveStereo(wavWriteBufferL.data(), wavWriteBufferR.data(),
                     wavWriteBufferInterleaved.data(), 512);
    enc.writeFrames(wavWriteBufferInterleaved.data(), nrFramesRead);
    i++;
  }

  if (nrFramesRead) {
    renderer.processAudioBufferStereo(wavReadBuffer.data(),
                                      wavWriteBufferL.data(),
                                      wavWriteBufferR.data(), nrFramesRead);

    // interleave L and R
    interleaveStereo(wavWriteBufferL.data(), wavWriteBufferR.data(),
                     wavWriteBufferInterleaved.data(), nrFramesRead);
    enc.writeFrames(wavWriteBufferInterleaved.data(), nrFramesRead);
  }

  audios::Vector3 a = {1.0, 0.0, 1.0, 0.0};
  auto b = a.toPolar();
  std::cout << b.x << " " << b.y << " " << b.z << "\n";

  return 0;
}
