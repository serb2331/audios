#include "FLACAudioFileDecoder.h"
#include "core/context/Context.h"
#include <iomanip>

bool FLACAudioFileDecoder::openFile(std::string fileName) {
  _p_drflac = drflac_open_file(fileName.c_str(), NULL);

  if (_p_drflac == NULL) {
    USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
    return false;
  }

  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void FLACAudioFileDecoder::reset() { drflac_seek_to_pcm_frame(_p_drflac, 0); }

void FLACAudioFileDecoder::logFileInformation() {
  if (_isFileInitialized) {
    USE_LOGGING("=== FLAC File Information ===\n");
    USE_LOGGING("Channels:          " << (int)_p_drflac->channels << "\n");
    USE_LOGGING("Sample Rate:       " << _p_drflac->sampleRate << " Hz\n");
    USE_LOGGING("Bits Per Sample:   " << (int)_p_drflac->bitsPerSample
                                      << " bits\n");
    USE_LOGGING("Total PCM Frames:  "
                << (unsigned long long)_p_drflac->totalPCMFrameCount << "\n");

    // Calculate duration
    double duration =
        (double)_p_drflac->totalPCMFrameCount / _p_drflac->sampleRate;
    USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                      << " seconds\n");
  }
}

FLACAudioFileDecoder::~FLACAudioFileDecoder() { drflac_close(_p_drflac); }