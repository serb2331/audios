#include "MP3AudioFileDecoder.h"
#include "core/context/Context.h"
#include <iomanip>

bool MP3AudioFileDecoder::openFile(std::string fileName) {
  _p_drmp3 = std::make_unique<drmp3>();
  if (!drmp3_init_file(_p_drmp3.get(), fileName.c_str(), NULL)) {
    USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
    return false;
  }
  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void MP3AudioFileDecoder::reset() {
  drmp3_seek_to_pcm_frame(_p_drmp3.get(), 0);
}

void MP3AudioFileDecoder::logFileInformation() {
  if (!_isFileInitialized)
    return;

  USE_LOGGING("=== MP3 File Information ===\n");
  USE_LOGGING("Channels:          " << _p_drmp3->channels << "\n");
  USE_LOGGING("Sample Rate:       " << _p_drmp3->sampleRate << " Hz\n");

  // This function forces the library to scan the file to calculate the total
  // length. It can be slightly slow on massive files, so only call it if you
  // need it.
  drmp3_uint64 totalFrames = drmp3_get_pcm_frame_count(_p_drmp3.get());

  USE_LOGGING("Total PCM Frames:  " << (unsigned long long)totalFrames << "\n");

  // Calculate duration
  double duration = (double)totalFrames / _p_drmp3->sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                    << " seconds\n");
}

MP3AudioFileDecoder::~MP3AudioFileDecoder() { drmp3_uninit(_p_drmp3.get()); }