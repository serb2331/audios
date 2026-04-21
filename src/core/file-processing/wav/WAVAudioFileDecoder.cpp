#include "WAVAudioFileDecoder.h"
#include "core/context/Context.h"
#include <memory>

bool WAVAudioFileDecoder::openFile(std::string fileName)
{
    _p_drwav = std::make_unique<drwav>();
    int error_code = drwav_init_file(_p_drwav.get(), fileName.c_str(), NULL);
    if (!error_code)
    {
        USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
        return false;
    }
    USE_LOGGING("Successfully opened file \"" << fileName << "\"");
    return true;
}

void WAVAudioFileDecoder::reset()
{
    drwav_seek_to_pcm_frame(_p_drwav.get(), 0);
}

WAVAudioFileDecoder::~WAVAudioFileDecoder()
{
    drwav_uninit(_p_drwav.get());
}