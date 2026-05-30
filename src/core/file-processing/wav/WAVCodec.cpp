#include "WAVCodec.h"
#include "../external/dr_wav.h"
#include "core/context/Context.h"
#include <cstddef>
#include <iomanip>
#include <memory>
#include <sys/types.h>
#include <vector>

struct WAVAudioFileDecoder::WAVAudioFileDecoderImpl {
  drwav drwav;
  std::string filePath = "";
  bool isFileInitialized = false;
};

WAVAudioFileDecoder::WAVAudioFileDecoder()
    : _impl(std::make_unique<WAVAudioFileDecoderImpl>()) {}

WAVAudioFileDecoder::~WAVAudioFileDecoder() {
  if (_impl->isFileInitialized)
    drwav_uninit(&_impl->drwav);
}

bool WAVAudioFileDecoder::initFile(std::string filePath) {
  int error_code = drwav_init_file(&_impl->drwav, filePath.c_str(), NULL);

  if (!error_code) {
    USE_LOGGING_ERROR("Couldn't open file with filename \"" << filePath
                                                            << "\"");
    return false;
  }

  _impl->isFileInitialized = true;
  _impl->filePath = filePath;
  USE_LOGGING("Successfully opened file \"" << filePath << "\"");
  return true;
}

void WAVAudioFileDecoder::resetPointer() {
  drwav_seek_to_pcm_frame(&_impl->drwav, 0);
}

void WAVAudioFileDecoder::resetDecoder() {
  if (!_impl->isFileInitialized)
    return;

  drwav_uninit(&_impl->drwav);
  _impl->filePath = "";
  _impl->isFileInitialized = false;
}

void WAVAudioFileDecoder::logFileInformation() {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Can't log details of uninitialized decoder.");
    return;
  }
  USE_LOGGING("=== WAV File Information ===");
  USE_LOGGING("File Path:         " << _impl->filePath);
  USE_LOGGING("Channels:          " << _impl->drwav.channels);
  USE_LOGGING("Sample Rate:       " << _impl->drwav.sampleRate << " Hz");
  USE_LOGGING("Bits Per Sample:   " << _impl->drwav.bitsPerSample << " bits");
  USE_LOGGING("Total PCM Frames:  "
              << (unsigned long long)_impl->drwav.totalPCMFrameCount);

  // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
  if (_impl->drwav.fmt.formatTag == DR_WAVE_FORMAT_PCM) {
    USE_LOGGING("Format:            Standard Integer PCM");
  } else if (_impl->drwav.fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
    USE_LOGGING("Format:            IEEE Floating Point");
  } else {
    USE_LOGGING("Format:            Compressed or Other ("
                << _impl->drwav.fmt.formatTag << ")");
  }

  // Duration = Total Frames / Sample Rate
  double durationSeconds =
      (double)_impl->drwav.totalPCMFrameCount / _impl->drwav.sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                    << " seconds");

  // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
  unsigned long long dataSizeBytes = _impl->drwav.totalPCMFrameCount *
                                     _impl->drwav.channels *
                                     (_impl->drwav.bitsPerSample / 8);
  USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                    << (double)dataSizeBytes / (1024 * 1024)
                                    << " MB)");
}

void WAVAudioFileDecoder::dumpContents(u_int32_t framesToDump) {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Error dumping contents of uninitialized decoder.")
    return;
  }

  drwav_seek_to_pcm_frame(&_impl->drwav, 0);

  std::vector<float> bufferOut(_impl->drwav.channels);
  u_int32_t currentFrame = 0;
  int readFrame = 1;

  while (currentFrame < framesToDump && readFrame == 1) {
    readFrame =
        drwav_read_pcm_frames_f32(&_impl->drwav, 1, bufferOut.data());

    std::stringstream ss("");
    ss << currentFrame << " ";
    for (auto value : bufferOut)
      ss << value << ' ';

    USE_LOGGING(ss.rdbuf());

    currentFrame += 1;
  }
}

WAVAudioFileFormat WAVAudioFileDecoder::getDataFormat() {
  return {.container = _impl->drwav.container,
          .format = _impl->drwav.translatedFormatTag,
          .channels = _impl->drwav.channels,
          .sampleRate = _impl->drwav.sampleRate,
          .bitsPerSample = _impl->drwav.bitsPerSample};
}

u_int32_t WAVAudioFileDecoder::getChannelNumber() {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Error getting channel number of uninitialized decoder.")
    return 0;
  }

  return _impl->drwav.channels;
}

u_int32_t WAVAudioFileDecoder::readFrames(float *frameBuffer,
                                          u_int32_t numFrames) {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Error reading contents of uninitialized decoder.")
    return 0;
  }

  u_int64_t readFrames =
      drwav_read_pcm_frames_f32(&_impl->drwav, numFrames, frameBuffer);
  return readFrames;
}

///////////////////

struct WAVAudioFileEncoder::WAVAudioFileEncoderImpl {
  drwav drwav;
  bool isFileInitialized = false;
  std::string filePath = "";
};

WAVAudioFileFormat WAVAudioFileEncoder::DefaultDataFormat() {
  // To be moved to context singleton
  return {
      .container = drwav_container_riff,
      .format = DR_WAVE_FORMAT_IEEE_FLOAT,
      .channels = 1,
      .sampleRate = 44100,
      .bitsPerSample = 32,
  };
}

bool WAVAudioFileEncoder::isInitialized() const {
  return _impl->isFileInitialized;
}

bool WAVAudioFileEncoder::initFile(std::string filePath) {
  auto format = WAVAudioFileEncoder::DefaultDataFormat();

  auto ret = drwav_init_file_write(
      &_impl->drwav, filePath.c_str(),
      reinterpret_cast<drwav_data_format *>(&format), NULL);
  if (!ret) {
    USE_LOGGING_ERROR("Couldnt initialize WAVEncoder for file: "
                      << filePath << " { " << ret << " } ");
    return false;
  }

  _impl->isFileInitialized = true;
  _impl->filePath = filePath;
  return true;
}

bool WAVAudioFileEncoder::initFile(std::string filePath, const void *format) {
  if (_impl->isFileInitialized) {
    USE_LOGGING_ERROR(
        "Attempting to init an encoder that is already initialized.");
    return false;
  }

  auto typedFormat = static_cast<const drwav_data_format *>(format);

  drwav_data_format floatFormat = {
      .container = typedFormat->container,
      .format = DR_WAVE_FORMAT_IEEE_FLOAT,
      .channels = typedFormat->channels,
      .sampleRate = typedFormat->sampleRate,
      .bitsPerSample = 32,
  };
  auto ret = drwav_init_file_write(&_impl->drwav, filePath.c_str(),
                                   &floatFormat, NULL);
  if (!ret) {
    USE_LOGGING_ERROR("Couldnt initialize WAVEncoder for file: "
                      << filePath << " { " << ret << " } ");
    return false;
  }

  _impl->isFileInitialized = true;
  _impl->filePath = filePath;
  return true;
}

void WAVAudioFileEncoder::resetEncoder() {
  if (!_impl->isFileInitialized)
    return;

  drwav_uninit(&_impl->drwav);
  _impl->isFileInitialized = false;
  _impl->filePath = "";
}

u_int32_t WAVAudioFileEncoder::writeFrames(const float *frameBuffer,
                                           u_int32_t numFrames) {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Can't write out to an uninitialized encoder.");
    return 0;
  }

  return drwav_write_pcm_frames(&_impl->drwav, numFrames, frameBuffer);
}

void WAVAudioFileEncoder::logFileInformation() {
  if (!_impl->isFileInitialized) {
    USE_LOGGING_ERROR("Can't log details of uninitialized decoder.");
    return;
  }
  USE_LOGGING("=== WAV File Information ===");
  USE_LOGGING("File Path:         " << _impl->filePath);
  USE_LOGGING("Channels:          " << _impl->drwav.channels);
  USE_LOGGING("Sample Rate:       " << _impl->drwav.sampleRate << " Hz");
  USE_LOGGING("Bits Per Sample:   " << _impl->drwav.bitsPerSample << " bits");
  USE_LOGGING("Total PCM Frames:  "
              << (unsigned long long)_impl->drwav.totalPCMFrameCount);

  // Check the format tag (1 = standard PCM integer, 3 = IEEE Float)
  if (_impl->drwav.fmt.formatTag == DR_WAVE_FORMAT_PCM) {
    USE_LOGGING("Format:            Standard Integer PCM");
  } else if (_impl->drwav.fmt.formatTag == DR_WAVE_FORMAT_IEEE_FLOAT) {
    USE_LOGGING("Format:            IEEE Floating Point");
  } else {
    USE_LOGGING("Format:            Compressed or Other ("
                << _impl->drwav.fmt.formatTag << ")");
  }

  // Duration = Total Frames / Sample Rate
  double durationSeconds =
      (double)_impl->drwav.totalPCMFrameCount / _impl->drwav.sampleRate;
  USE_LOGGING("Duration:          " << std::setprecision(2) << durationSeconds
                                    << " seconds");

  // Audio Data Size = Frames * Channels * (Bits per Sample / 8)
  unsigned long long dataSizeBytes = _impl->drwav.totalPCMFrameCount *
                                     _impl->drwav.channels *
                                     (_impl->drwav.bitsPerSample / 8);
  USE_LOGGING("Audio Payload:     " << dataSizeBytes << " bytes ("
                                    << (double)dataSizeBytes / (1024 * 1024)
                                    << " MB)");
}

WAVAudioFileEncoder::~WAVAudioFileEncoder() {
  if (_impl->isFileInitialized)
    drwav_uninit(&_impl->drwav);
}