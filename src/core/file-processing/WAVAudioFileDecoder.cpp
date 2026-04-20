#include "WAVAudioFileDecoder.h"
#include "core/context/Context.h"

bool WAVAudioFileDecoder::openFile(std::string fileName)
{
    if (!drwav_init_file(_p_drwav, fileName.c_str(), NULL))
    {
        USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
        return false;
    }
    USE_LOGGING("Successfully opened file \"" << fileName << "\"");
    return true;
}

void WAVAudioFileDecoder::reset()
{
    drwav_seek_to_pcm_frame(_p_drwav, 0);
}

WAVAudioFileDecoder::~WAVAudioFileDecoder()
{
    drwav_uninit(_p_drwav);
}