#pragma once

#include "../../../external/dr_wav.h"
#include "core/interfaces/file_processing.h"
#include <memory>

class WAVAudioFileDecoder : public IAudioFileDecoder
{
private:

    std::unique_ptr<drwav>_p_drwav;

public:

    bool openFile (std::string filePath) override;

    void reset() override;

    ~WAVAudioFileDecoder();
};