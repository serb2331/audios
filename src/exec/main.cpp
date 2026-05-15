#include "../lib/audio-library.h"
#include "core/context/Context.h"
#include "core/file-processing/mp3/MP3AudioFileCodec.h"
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
  const u_int32_t bufferSize = Context::GetInstance().getBufferFrameCount();

  /////////////////////////////////////

  // auto wavDecodecUP = std::make_unique<WAVAudioFileCodec>();
  // wavDecodecUP->openFile("./sounds/pluck.wav");

  // auto wavFilter = std::make_unique<GainAudioFilter>(1.0,
  // std::move(wavDecodecUP)); auto channelCount =
  // wavFilter->getChannelNumber(); std::vector<float> exampleBuffer(bufferSize
  // * channelCount, 0.0); auto readFrames =
  // wavFilter->readFrames(exampleBuffer.data(), bufferSize);

  //////////////////////////////////

  // auto mp3_decodec = std::make_unique<MP3AudioFileCodec>();
  // mp3_decodec->openFile("./sounds/pluck.mp3");
  // std::cout << "\nmp3\n\n";
  // mp3_decodec->dumpContents(10);
  // mp3_decodec->reset();

  // auto mp3Filter =
  //     std::make_unique<GainAudioFilter>(1.0, std::move(mp3_decodec));
  // auto channelCount = mp3Filter->getChannelNumber();

  // std::vector<float> exampleBuffer(bufferSize * channelCount, 0.0);
  // auto readFrames = mp3Filter->readFrames(exampleBuffer.data(), bufferSize);

  //////////////////////////////////////

  // auto flacDecodec = std::make_unique<FLACAudioFileCodec>();
  // flacDecodec->openFile("./sounds/pluck.flac");
  // std::cout << "\nflac\n\n";
  // flacDecodec->dumpContents(10);
  // flacDecodec->reset();

  // auto flacFilter =
  //     std::make_unique<GainAudioFilter>(0.5, std::move(flacDecodec));
  // auto channelCount = flacFilter->getChannelNumber();

  // std::vector<float> exampleBuffer(bufferSize * channelCount, 0.0);
  // auto readFrames = flacFilter->readFrames(exampleBuffer.data(), bufferSize);

  /////////////////////////////////////
  
  // USE_LOGGING("FRAMES READ:" << readFrames);
  // for (int i = 0; i < 10; i += 1) {
  //   std::stringstream s("");
  //   for (u_int32_t j = 0; j < channelCount; j += 1)
  //     s << exampleBuffer[i * channelCount + j] << " ";

  //   USE_LOGGING(i << " " << s.rdbuf());
  // }

  return 0;
}
