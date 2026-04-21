#pragma once

#include "../../../external/dr_mp3.h"
#include "core/interfaces/file_processing.h"
#include <memory>

class MP3AudioFileDecoder : public IAudioFileDecoder {
private:
  std::unique_ptr<drmp3> _p_drmp3;
  bool _isFileInitialized = false;

public:
  bool openFile(std::string filePath) override;

  void reset() override;

  void logFileInformation() override;

  ~MP3AudioFileDecoder();
};