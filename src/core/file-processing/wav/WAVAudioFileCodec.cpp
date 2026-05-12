#include "WAVAudioFileCodec.h"
#include "core/context/Context.h"
#include <iomanip>
#include <memory>
#include <sys/types.h>

bool WAVAudioFileCodec::openFile(std::string fileName) {
  _drwavP = std::make_unique<drwav>();
  int error_code = drwav_init_file(_drwavP.get(), fileName.c_str(), NULL);

  if (!error_code) {
    USE_LOGGING_ERROR("Couldn't open file with filename \"" << fileName
                                                            << "\"");
    return false;
  }

  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void WAVAudioFileCodec::reset() { drwav_seek_to_pcm_frame(_drwavP.get(), 0); }

void WAVAudioFileCodec::logFileInformation() {
  if (_isFileInitialized) {
    USE_LOGGING("=== WAV File Information ===\n");
    USE_LOGGING("Channels:          " << _drwavP->channels << "\n");
    USE_LOGGING("Sample Rate:       " << _drwavP->sampleRate << " Hz\n");
    USE_LOGGING("Bits Per Sample:   " << _drwavP->bitsPerSample << " bits\n");

    USE_LOGGING("Total PCM Frames:  "
                << (unsigned long long)_drwavP->totalPCMFrameCount << "\n");

    // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
    if (_drwavP->fmt.formatTag == DR_WAVE_FORMAT_PCM) {
      USE_LOGGING("Format:            Standard Integer PCM\n");
    } else if (_drwavP->fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
      USE_LOGGING("Format:            IEEE Floating Point\n");
    } else {
      USE_LOGGING("Format:            Compressed or Other ("
                  << _drwavP->fmt.formatTag << ")\n");
    }

    // Duration = Total Frames / Sample Rate
    double durationSeconds =
        (double)_drwavP->totalPCMFrameCount / _drwavP->sampleRate;
    USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                      << " seconds\n");

    // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
    unsigned long long dataSizeBytes = _drwavP->totalPCMFrameCount *
                                       _drwavP->channels *
                                       (_drwavP->bitsPerSample / 8);
    USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                      << (double)dataSizeBytes / (1024 * 1024)
                                      << " MB)\n");
  }
  // throw an error in the future
}

void WAVAudioFileCodec::dumpContents(u_int32_t framesToDump) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drwav_seek_to_pcm_frame(_drwavP.get(), 0);

  float bufferOut;
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame = drwav_read_pcm_frames_f32(_drwavP.get(), 1, &bufferOut);
    USE_LOGGING(currentFrame << " " << readFrame << " " << bufferOut);
    currentFrame += 1;
  }
}

WAVAudioFileCodec::~WAVAudioFileCodec() { drwav_uninit(_drwavP.get()); }