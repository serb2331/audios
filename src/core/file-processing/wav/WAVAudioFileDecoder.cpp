#include "WAVAudioFileDecoder.h"
#include "core/context/Context.h"
#include <iomanip>
#include <memory>

bool WAVAudioFileDecoder::openFile(std::string fileName) {
  _p_drwav = std::make_unique<drwav>();
  int error_code = drwav_init_file(_p_drwav.get(), fileName.c_str(), NULL);

  if (!error_code) {
    USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
    return false;
  }

  _isFileInitialized = true;
  USE_LOGGING("Successfully opened file \"" << fileName << "\"");
  return true;
}

void WAVAudioFileDecoder::reset() {
  drwav_seek_to_pcm_frame(_p_drwav.get(), 0);
}

void WAVAudioFileDecoder::logFileInformation() {
  if (_isFileInitialized) {
    USE_LOGGING("=== WAV File Information ===\n");
    USE_LOGGING("Channels:          " << _p_drwav->channels << "\n");
    USE_LOGGING("Sample Rate:       " << _p_drwav->sampleRate << " Hz\n");
    USE_LOGGING("Bits Per Sample:   " << _p_drwav->bitsPerSample << " bits\n");

    USE_LOGGING("Total PCM Frames:  "
                << (unsigned long long)_p_drwav->totalPCMFrameCount << "\n");

    // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
    if (_p_drwav->fmt.formatTag == DR_WAVE_FORMAT_PCM) {
      USE_LOGGING("Format:            Standard Integer PCM\n");
    } else if (_p_drwav->fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
      USE_LOGGING("Format:            IEEE Floating Point\n");
    } else {
      USE_LOGGING("Format:            Compressed or Other ("
                  << _p_drwav->fmt.formatTag << ")\n");
    }

    // Duration = Total Frames / Sample Rate
    double durationSeconds =
        (double)_p_drwav->totalPCMFrameCount / _p_drwav->sampleRate;
    USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                      << " seconds\n");

    // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
    unsigned long long dataSizeBytes = _p_drwav->totalPCMFrameCount *
                                       _p_drwav->channels *
                                       (_p_drwav->bitsPerSample / 8);
    USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                      << (double)dataSizeBytes / (1024 * 1024)
                                      << " MB)\n");
  }
  // throw an error in the future
}

WAVAudioFileDecoder::~WAVAudioFileDecoder() { drwav_uninit(_p_drwav.get()); }