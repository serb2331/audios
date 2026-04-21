#include "MP3AudioFileDecoder.h"
#include "core/context/Context.h"

bool MP3AudioFileDecoder::openFile(std::string fileName)
{
    _p_drmp3 = std::make_unique<drmp3>();
    if (!drmp3_init_file(_p_drmp3.get(), fileName.c_str(), NULL))
    {
        USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
        return false;
    }
    USE_LOGGING("Successfully opened file \"" << fileName << "\"");
    return true;
}

void MP3AudioFileDecoder::reset()
{
    drmp3_seek_to_pcm_frame(_p_drmp3.get(), 0);
}

MP3AudioFileDecoder::~MP3AudioFileDecoder()
{
    drmp3_uninit(_p_drmp3.get());
}