#pragma once

#include "../../external/dr_wav.h"
#include "../interfaces/file_processing.h"

class WAVAudioFileDecoder : public IAudioFileDecoder
{
private:

    drwav *_p_drwav = nullptr;

public:

    bool openFile (std::string filePath) override;

    void reset() override;

    ~WAVAudioFileDecoder();
};