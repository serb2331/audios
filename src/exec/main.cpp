#include "core/file-processing/flac/FLACAudioFileDecoder.h"
#include "lib/audio-library.h"
#include <stdio.h>

#define PROJECT_NAME "audio-library"

static void setupProject() { Context::GetInstance().setLogging(true); }

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("%s takes no arguments.\n", argv[0]);
    return 1;
  }
  printf("\n\n\nThis is project %s.\n", PROJECT_NAME);

  setupProject();

  WAVAudioFileCodec wav_decodec = WAVAudioFileCodec();
  wav_decodec.openFile("./sounds/pluck.wavvv");
  wav_decodec.logFileInformation();
  std::cout << "wav\n\n";
  wav_decodec.dumpContents(10);

  MP3AudioFileCodec mp3_decodec = MP3AudioFileCodec();
  mp3_decodec.openFile("./sounds/pluck.mp3");
  mp3_decodec.logFileInformation();
  std::cout << "\nmp3\n\n";
  mp3_decodec.dumpContents(10);

  FLACAudioFileCodec flac_decodec = FLACAudioFileCodec();
  flac_decodec.openFile("./sounds/pluck.flac");
  flac_decodec.logFileInformation();
  std::cout << "\nflac\n\n";
  flac_decodec.dumpContents(10);

  return 0;
}
