#include "FLACAudioFileCodec.h"
#include "core/context/Context.h"
#include <iomanip>
#include <vector>

bool FLACAudioFileCodec::openFile(std::string fileName) {
  _drflacP = drflac_open_file(fileName.c_str(), NULL);

  if (_drflacP == NULL) {
    USE_LOGGING_ERROR("Couldn't open file with filename \"" << fileName
                                                            << "\"");
    return false;
  }

  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void FLACAudioFileCodec::reset() { drflac_seek_to_pcm_frame(_drflacP, 0); }

void FLACAudioFileCodec::logFileInformation() {
  if (_isFileInitialized) {
    USE_LOGGING("=== FLAC File Information ===\n");
    USE_LOGGING("Channels:          " << (int)_drflacP->channels << "\n");
    USE_LOGGING("Sample Rate:       " << _drflacP->sampleRate << " Hz\n");
    USE_LOGGING("Bits Per Sample:   " << (int)_drflacP->bitsPerSample
                                      << " bits\n");
    USE_LOGGING("Total PCM Frames:  "
                << (unsigned long long)_drflacP->totalPCMFrameCount << "\n");

    // Calculate duration
    double duration =
        (double)_drflacP->totalPCMFrameCount / _drflacP->sampleRate;
    USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                      << " seconds\n");
  }
}

void FLACAudioFileCodec::dumpContents(u_int32_t framesToDump) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drflac_seek_to_pcm_frame(_drflacP, 0);

  USE_LOGGING((int)_drflacP->channels);
  std::vector<float> bufferOut(_drflacP->channels);
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame = drflac_read_pcm_frames_f32(_drflacP, 1, bufferOut.data());
    USE_LOGGING(currentFrame << " " << readFrame << " ");
    for (auto value : bufferOut)
      USE_LOGGING(value);

    currentFrame += 1;
  }
}

FLACAudioFileCodec::~FLACAudioFileCodec() { drflac_close(_drflacP); }