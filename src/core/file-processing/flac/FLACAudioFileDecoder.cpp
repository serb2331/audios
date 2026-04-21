#include "FLACAudioFileDecoder.h"
#include "core/context/Context.h"

bool FLACAudioFileDecoder::openFile(std::string fileName)
{
    _p_drflac = drflac_open_file(fileName.c_str(), NULL);
    if (_p_drflac == NULL)
    {
        USE_LOGGING("Couldn't open file with filename \"" << fileName << "\"");
        return false;
    }
    USE_LOGGING("Successfully opened file \"" << fileName << "\"");
    return true;
}

void FLACAudioFileDecoder::reset()
{
    drflac_seek_to_pcm_frame(_p_drflac, 0);
}

FLACAudioFileDecoder::~FLACAudioFileDecoder()
{
    drflac_close(_p_drflac);
}