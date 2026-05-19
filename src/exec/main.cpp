#include "../lib/audio-library.h"
#include "core/context/Context.h"
#include "core/file-processing/wav/WAVCodec.h"
#include "core/filtering/filters.h"
#include <memory>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <vector>

#define PROJECT_NAME "audio-library"

static void setupProject() {
  Context::GetInstance().setLogging(true);
  Context::GetInstance().setBufferFrameCount(512);
}

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("%s takes no arguments.\n", argv[0]);
    return 1;
  }
  printf("\n\n\nThis is project %s.\n", PROJECT_NAME);

  setupProject();
  /////////////////////////////////////

  auto decoder = std::make_unique<WAVAudioFileDecoder>();
  decoder->initFile("/home/serb/coding/licenta/audio-lib/build/sounds/"
                    "qubodup-cfork-ccby3-jump.wav");
  decoder->logFileInformation();

  //

  const u_int32_t bufferSize = Context::GetInstance().getBufferFrameCount();
  auto decoderFormat = decoder->getDataFormat();

  auto encoder = std::make_unique<WAVAudioFileEncoder>();
  encoder->initFile("./sounds/writtenWAV.wav", &decoderFormat);

  auto wavFilter = std::make_unique<GainAudioFilter>(0.5, std::move(decoder));
  wavFilter->execute(bufferSize, *encoder);

  std::cout << "\n\n";
  encoder->logFileInformation();
  encoder.reset();
  //

  std::cout << "\n\n";
  decoder = std::make_unique<WAVAudioFileDecoder>();
  decoder->initFile("./sounds/writtenWAV.wav");
  decoder->logFileInformation();

  return 0;
}
