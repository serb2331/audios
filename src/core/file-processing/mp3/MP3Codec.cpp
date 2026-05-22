#include "MP3Codec.h"
#include "core/context/Context.h"
#include <iomanip>
#include <vector>

bool MP3AudioFileDecoder::initFile(std::string fileName) {
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

void MP3AudioFileDecoder::resetPointer() {
  drmp3_seek_to_pcm_frame(_drmp3P.get(), 0);
}

void MP3AudioFileDecoder::logFileInformation() {
  if (!_isFileInitialized)
    return;

  USE_LOGGING("=== MP3 File Information ===");
  USE_LOGGING("Channels:          " << _drmp3P->channels);
  USE_LOGGING("Sample Rate:       " << _drmp3P->sampleRate << " Hz");

  // This function forces the library to scan the file to calculate the total
  // length. It can be slightly slow on massive files, so only call it if you
  // need it.
  drmp3_uint64 totalFrames = drmp3_get_pcm_frame_count(_drmp3P.get());

  USE_LOGGING("Total PCM Frames:  " << (unsigned long long)totalFrames);

  // Calculate duration
  double duration = (double)totalFrames / _drmp3P->sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << duration
                                    << " seconds");
}

void MP3AudioFileDecoder::dumpContents(u_int32_t framesToDump) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drmp3_seek_to_pcm_frame(_drmp3P.get(), 0);

  USE_LOGGING((int)_drmp3P->channels);
  std::vector<float> bufferOut(_drmp3P->channels);
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame = drmp3_read_pcm_frames_f32(_drmp3P.get(), 1, bufferOut.data());
    USE_LOGGING(currentFrame << " " << readFrame);

    std::stringstream ss("");
    for (auto value : bufferOut)
      ss << value << ' ';
    USE_LOGGING(ss.rdbuf());

    currentFrame += 1;
  }
}

//

u_int32_t MP3AudioFileDecoder::getChannelNumber() {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error getting channel number of uninitialized decoder.")
    return 0;
  }

  return _drmp3P->channels;
}

u_int32_t MP3AudioFileDecoder::readFrames(float *frameBuffer,
                                          u_int32_t numFrames) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error reading frames of uninitialized decoder.");
  }

  u_int64_t readFrames =
      drmp3_read_pcm_frames_f32(_drmp3P.get(), numFrames, frameBuffer);

  return readFrames;
}

//

MP3AudioFileDecoder::~MP3AudioFileDecoder() { drmp3_uninit(_drmp3P.get()); }