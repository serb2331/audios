#include "FLACCodec.h"
#include <iomanip>
#include <vector>

bool FLACAudioFileDecoder::initFile(std::string fileName) {
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

void FLACAudioFileDecoder::resetPointer() {
  drflac_seek_to_pcm_frame(_drflacP, 0);
}

void FLACAudioFileDecoder::logFileInformation() {
  if (_isFileInitialized) {
    USE_LOGGING("=== FLAC File Information ===");
    USE_LOGGING("Channels:          " << (int)_drflacP->channels);
    USE_LOGGING("Sample Rate:       " << _drflacP->sampleRate << " Hz");
    USE_LOGGING("Bits Per Sample:   " << (int)_drflacP->bitsPerSample
                                      << " bits");
    USE_LOGGING("Total PCM Frames:  "
                << (unsigned long long)_drflacP->totalPCMFrameCount << "");

    // Calculate duration
    double duration =
        (double)_drflacP->totalPCMFrameCount / _drflacP->sampleRate;
    USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                      << " seconds");
  }
}

void FLACAudioFileDecoder::dumpContents(u_int32_t framesToDump) {
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
    USE_LOGGING(currentFrame << " " << readFrame);

    std::stringstream ss("");
    for (auto value : bufferOut)
      ss << value << ' ';
    USE_LOGGING(ss.rdbuf());

    currentFrame += 1;
  }
}

//

u_int32_t FLACAudioFileDecoder::readFrames(float *frameBuffer,
                                           u_int32_t numFrames) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error reading frames of uninitialized decoder.");
  }

  u_int64_t readFrames =
      drflac_read_pcm_frames_f32(_drflacP, numFrames, frameBuffer);

  return readFrames;
}

u_int32_t FLACAudioFileDecoder::getChannelNumber() {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error getting channel number of uninitialized decoder.")
    return 0;
  }

  return _drflacP->channels;
}

//

FLACAudioFileDecoder::~FLACAudioFileDecoder() { drflac_close(_drflacP); }