#include "../lib/audio-library.h"
#include "core/context/Context.h"
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

  auto wavDecodecUP = std::make_unique<WAVAudioFileCodec>();
  wavDecodecUP->openFile("./sounds/pluck.wav");
  auto wavDecodecP = wavDecodecUP.get();

  std::cout << "wav before filter\n\n";
  wavDecodecP->logFileInformation();
  wavDecodecP->dumpContents(10);
  wavDecodecP->reset();

  const u_int32_t bufferSize = Context::GetInstance().getBufferFrameCount();

  auto filter = std::make_unique<GainAudioFilter>(1.0, std::move(wavDecodecUP));

  auto channelCount = filter->getChannelNumber();

  std::vector<float> exampleBuffer(bufferSize * channelCount, 0.0);
  auto readFrames = filter->readFrames(exampleBuffer.data(), bufferSize);

  USE_LOGGING("FRAMES READ:" << readFrames);
  for (int i = 0; i < 10; i += 1) {
    std::stringstream s("");
    for (u_int32_t j = 0; j < channelCount; j += 1)
      s << exampleBuffer[i * channelCount + j] << " ";

    USE_LOGGING(i << " " << s.rdbuf());
  }

  // MP3AudioFileCodec mp3_decodec = MP3AudioFileCodec();
  // mp3_decodec.openFile("./sounds/pluck.mp3");
  // mp3_decodec.logFileInformation();
  // std::cout << "\nmp3\n\n";
  // mp3_decodec.dumpContents(10);

  // FLACAudioFileCodec flac_decodec = FLACAudioFileCodec();
  // flac_decodec.openFile("./sounds/pluck.flac");
  // flac_decodec.logFileInformation();
  // std::cout << "\nflac\n\n";
  // flac_decodec.dumpContents(10);

  return 0;
}
