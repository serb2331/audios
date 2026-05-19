#include "WAVCodec.h"
#include "core/context/Context.h"
#include <cstddef>
#include <iomanip>
#include <memory>
#include <sys/types.h>
#include <vector>

bool WAVAudioFileDecoder::initFile(std::string filePath) {
  _pdrwav = std::make_unique<drwav>();
  int error_code = drwav_init_file(_pdrwav.get(), filePath.c_str(), NULL);

  if (!error_code) {
    USE_LOGGING_ERROR("Couldn't open file with filename \"" << filePath
                                                            << "\"");
    return false;
  }

  _isFileInitialized = true;
  _filePath = filePath;
  USE_LOGGING("Successfully opened file \"" << filePath << "\"");
  return true;
}

void WAVAudioFileDecoder::resetPointer() {
  drwav_seek_to_pcm_frame(_pdrwav.get(), 0);
}

void WAVAudioFileDecoder::resetDecoder() {
  if (!_isFileInitialized)
    return;

  drwav_uninit(_pdrwav.get());
  _pdrwav = nullptr;
  _filePath = "";
  _isFileInitialized = false;
}

void WAVAudioFileDecoder::logFileInformation() {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Can't log details of uninitialized decoder.");
    return;
  }
  USE_LOGGING("=== WAV File Information ===");
  USE_LOGGING("File Path:         " << _filePath);
  USE_LOGGING("Channels:          " << _pdrwav->channels);
  USE_LOGGING("Sample Rate:       " << _pdrwav->sampleRate << " Hz");
  USE_LOGGING("Bits Per Sample:   " << _pdrwav->bitsPerSample << " bits");
  USE_LOGGING(
      "Total PCM Frames:  " << (unsigned long long)_pdrwav->totalPCMFrameCount);

  // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
  if (_pdrwav->fmt.formatTag == DR_WAVE_FORMAT_PCM) {
    USE_LOGGING("Format:            Standard Integer PCM");
  } else if (_pdrwav->fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
    USE_LOGGING("Format:            IEEE Floating Point");
  } else {
    USE_LOGGING("Format:            Compressed or Other ("
                << _pdrwav->fmt.formatTag << ")");
  }

  // Duration = Total Frames / Sample Rate
  double durationSeconds =
      (double)_pdrwav->totalPCMFrameCount / _pdrwav->sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                    << " seconds");

  // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
  unsigned long long dataSizeBytes = _pdrwav->totalPCMFrameCount *
                                     _pdrwav->channels *
                                     (_pdrwav->bitsPerSample / 8);
  USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                    << (double)dataSizeBytes / (1024 * 1024)
                                    << " MB)");
}

void WAVAudioFileDecoder::dumpContents(u_int32_t framesToDump) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drwav_seek_to_pcm_frame(_pdrwav.get(), 0);

  std::vector<float> bufferOut(_pdrwav->channels);
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame = drwav_read_pcm_frames_f32(_pdrwav.get(), 1, bufferOut.data());

    std::stringstream ss("");
    ss << currentFrame << " ";
    for (auto value : bufferOut)
      ss << value << ' ';

    USE_LOGGING(ss.rdbuf());

    currentFrame += 1;
  }
}

drwav_data_format WAVAudioFileDecoder::getDataFormat() {
  return {.container = _pdrwav->container,
          .format = _pdrwav->translatedFormatTag,
          .channels = _pdrwav->channels,
          .sampleRate = _pdrwav->sampleRate,
          .bitsPerSample = _pdrwav->bitsPerSample};
}

u_int32_t WAVAudioFileDecoder::getChannelNumber() {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error getting channel number of uninitialized decoder.")
    return 0;
  }

  return _pdrwav->channels;
}

u_int32_t WAVAudioFileDecoder::readFrames(float *frameBuffer,
                                          u_int32_t numFrames) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Error reading contents of uninitialized decoder.")
    return 0;
  }

  u_int64_t readFrames =
      drwav_read_pcm_frames_f32(_pdrwav.get(), numFrames, frameBuffer);
  return readFrames;
}

WAVAudioFileDecoder::~WAVAudioFileDecoder() {
  if (_pdrwav)
    drwav_uninit(_pdrwav.get());
}

///////////////////

drwav_data_format WAVAudioFileEncoder::DefaultDataFormat() {
  // To be moved to context singleton
  return {
      .container = drwav_container_riff,
      .format = DR_WAVE_FORMAT_IEEE_FLOAT,
      .channels = 1,
      .sampleRate = 44100,
      .bitsPerSample = 32,
  };
}

bool WAVAudioFileEncoder::isInitialized() const { return _isFileInitialized; }

bool WAVAudioFileEncoder::initFile(std::string filePath) {
  _pdrwav = std::make_unique<drwav>();
  auto format = WAVAudioFileEncoder::DefaultDataFormat();

  auto ret =
      drwav_init_file_write(_pdrwav.get(), filePath.c_str(), &format, NULL);
  if (!ret) {
    USE_LOGGING_ERROR("Couldnt initialize WAVEncoder for file: "
                      << filePath << " { " << ret << " } ");
    return false;
  }

  _isFileInitialized = true;
  _filePath = filePath;
  return true;
}

bool WAVAudioFileEncoder::initFile(std::string filePath, const void *format) {
  if (_isFileInitialized) {
    USE_LOGGING_ERROR(
        "Attempting to init an encoder that is already initialized.");
    return false;
  }

  _pdrwav = std::make_unique<drwav>();
  auto typedFormat = static_cast<const drwav_data_format *>(format);

  drwav_data_format floatFormat = {
      .container = typedFormat->container,
      .format = DR_WAVE_FORMAT_IEEE_FLOAT,
      .channels = typedFormat->channels,
      .sampleRate = typedFormat->sampleRate,
      .bitsPerSample = 32,
  };
  auto ret = drwav_init_file_write(_pdrwav.get(), filePath.c_str(),
                                   &floatFormat, NULL);
  if (!ret) {
    USE_LOGGING_ERROR("Couldnt initialize WAVEncoder for file: "
                      << filePath << " { " << ret << " } ");
    return false;
  }

  _isFileInitialized = true;
  _filePath = filePath;
  return true;
}

void WAVAudioFileEncoder::resetEncoder() {
  if (!_isFileInitialized)
    return;

  drwav_uninit(_pdrwav.get());
  _pdrwav = nullptr;
  _isFileInitialized = false;
  _filePath = "";
}

u_int32_t WAVAudioFileEncoder::writeFrames(const float *frameBuffer,
                                           u_int32_t numFrames) {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Can't write out to an uninitialized encoder.");
    return 0;
  }

  return drwav_write_pcm_frames(_pdrwav.get(), numFrames, frameBuffer);
}

void WAVAudioFileEncoder::logFileInformation() {
  if (!_isFileInitialized) {
    USE_LOGGING_ERROR("Can't log details of uninitialized decoder.");
    return;
  }
  USE_LOGGING("=== WAV File Information ===");
  USE_LOGGING("File Path:         " << _filePath);
  USE_LOGGING("Channels:          " << _pdrwav->channels);
  USE_LOGGING("Sample Rate:       " << _pdrwav->sampleRate << " Hz");
  USE_LOGGING("Bits Per Sample:   " << _pdrwav->bitsPerSample << " bits");
  USE_LOGGING(
      "Total PCM Frames:  " << (unsigned long long)_pdrwav->totalPCMFrameCount);

  // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
  if (_pdrwav->fmt.formatTag == DR_WAVE_FORMAT_PCM) {
    USE_LOGGING("Format:            Standard Integer PCM");
  } else if (_pdrwav->fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
    USE_LOGGING("Format:            IEEE Floating Point");
  } else {
    USE_LOGGING("Format:            Compressed or Other ("
                << _pdrwav->fmt.formatTag << ")");
  }

  // Duration = Total Frames / Sample Rate
  double durationSeconds =
      (double)_pdrwav->totalPCMFrameCount / _pdrwav->sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                    << " seconds");

  // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
  unsigned long long dataSizeBytes = _pdrwav->totalPCMFrameCount *
                                     _pdrwav->channels *
                                     (_pdrwav->bitsPerSample / 8);
  USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                    << (double)dataSizeBytes / (1024 * 1024)
                                    << " MB)");
}

WAVAudioFileEncoder::~WAVAudioFileEncoder() {
  if (_pdrwav)
    drwav_uninit(_pdrwav.get());
}