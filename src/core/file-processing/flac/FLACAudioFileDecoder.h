#pragma once

#include "../../../external/dr_flac.h"
#include "core/interfaces/file_processing.h"

class FLACAudioFileDecoder : public IAudioFileDecoder {
private:
  drflac *_p_drflac = nullptr;
  bool _isFileInitialized = false;

public:
  bool openFile(std::string filePath) override;

  void reset() override;

  void logFileInformation() override;

  ~FLACAudioFileDecoder();
};