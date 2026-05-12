#include "MP3AudioFileCodec.h"
#include "core/context/Context.h"
#include <iomanip>

bool MP3AudioFileCodec::openFile(std::string fileName) {
  _drmp3P = std::make_unique<drmp3>();
  if (!drmp3_init_file(_drmp3P.get(), fileName.c_str(), NULL)) {
    USE_LOGGING_ERROR("Couldn't open file with filename \"" << fileName
                                                            << "\"");
    return false;
  }
  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void MP3AudioFileCodec::reset() { drmp3_seek_to_pcm_frame(_drmp3P.get(), 0); }

void MP3AudioFileCodec::logFileInformation() {
  if (!_isFileInitialized)
    return;

  USE_LOGGING("=== MP3 File Information ===\n");
  USE_LOGGING("Channels:          " << _drmp3P->channels << "\n");
  USE_LOGGING("Sample Rate:       " << _drmp3P->sampleRate << " Hz\n");

  // This function forces the library to scan the file to calculate the total
  // length. It can be slightly slow on massive files, so only call it if you
  // need it.
  drmp3_uint64 totalFrames = drmp3_get_pcm_frame_count(_drmp3P.get());

  USE_LOGGING("Total PCM Frames:  " << (unsigned long long)totalFrames << "\n");

  // Calculate duration
  double duration = (double)totalFrames / _drmp3P->sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                    << " seconds\n");
}

void MP3AudioFileCodec::dumpContents(u_int32_t framesToDump) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drmp3_seek_to_pcm_frame(_drmp3P.get(), 0);

  float bufferOut;
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame = drmp3_read_pcm_frames_f32(_drmp3P.get(), 1, &bufferOut);
    USE_LOGGING(currentFrame << " " << readFrame << " " << bufferOut);
    currentFrame += 1;
  }
}

MP3AudioFileCodec::~MP3AudioFileCodec() { drmp3_uninit(_drmp3P.get()); }