#pragma once

#include "../../../external/dr_flac.h"
#include "core/interfaces/file_processing.h"

class FLACAudioFileCodec : public IAudioFileCodec {
private:
  drflac *_drflacP = nullptr;
  bool _isFileInitialized = false;

public:
  bool openFile(std::string filePath) override;

  void reset() override;

  void logFileInformation() override;

  void dumpContents(u_int32_t framesToDump) override;

  ~FLACAudioFileCodec();
};