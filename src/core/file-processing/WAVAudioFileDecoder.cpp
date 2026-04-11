#include "WAVAudioFileDecoder.h"
#include "core/context/Context.h"
#include <cstdint>
#include <ostream>
#include <array>
#include <bit>
#include <vector>

bool WAVAudioFileDecoder::openFile(std::string filePath)
{
    USE_LOGGING("AudioFile Decoder - opening file " << filePath);
    _inputFileStream = std::ifstream{filePath, std::ios::binary};

    if (!_inputFileStream.is_open())
        USE_LOGGING("AudioFile Decoder - failed opening file");

    return _inputFileStream.is_open();
}

char WAVAudioFileDecoder::getByte()
{
    char a;
    _inputFileStream >> a;
    return a;
}

void WAVAudioFileDecoder::resetFileStream()
{
    _inputFileStream.seekg(0);
    _inputFileStream.clear();
}

bool WAVAudioFileDecoder::validateHeader()
{
    // Validate Master RIFF Chunk

    std::array<char, _RIFF_HEADER_LENGTH> riffHeader;

    _inputFileStream.read(riffHeader.data(), _RIFF_HEADER_LENGTH);

    if (_inputFileStream.gcount() < (long)_RIFF_HEADER_LENGTH)
    {
        USE_LOGGING("AudioFile Decoder - Opened file is smaller than the needed header for WAV");
        return false;
    }

    if (riffHeader != _RIFF_HEADER)
    {
        USE_LOGGING("AudioFile Decoder - Opened file doesn't have a RIFF header");
        return false;
    }

    //

    std::array<char, 4> fileSizeBuffer;
    _inputFileStream.read(fileSizeBuffer.data(), 4);
    
    std::uint32_t fileSize = std::bit_cast<std::uint32_t>(fileSizeBuffer);

    USE_LOGGING("AudioFile Decoder - read file size: " << fileSize);

    //

    std::array<char, _WAVE_HEADER_LENGTH> waveHeader;

    _inputFileStream.read(waveHeader.data(), _WAVE_HEADER_LENGTH);

    if (_inputFileStream.gcount() < (long)_WAVE_HEADER_LENGTH)
    {
        USE_LOGGING("AudioFile Decoder - Opened file is smaller than the needed header for WAV");
        return false;
    }

    if (waveHeader != _WAVE_HEADER)
    {
        USE_LOGGING("AudioFile Decoder - Opened file doesn't have a WAVE header");
        return false;
    }

    // Validate data format chunk
    
    std::array<char, _FMT_HEADER_LENGTH> fmtHeader;

    _inputFileStream.read(fmtHeader.data(), _FMT_HEADER_LENGTH);

    if (_inputFileStream.gcount() < (long)_FMT_HEADER_LENGTH)
    {
        USE_LOGGING("AudioFile Decoder - Opened file is smaller than the needed format chunk header for WAV");
        return false;
    }
    
    if (fmtHeader != _FMT_HEADER)
    {
        USE_LOGGING("AudioFile Decoder - Opened file doesn't have an FMT header in format chunk");
        return false;
    }

    //

    std::array<char, 4> blocSizeBuffer;

    _inputFileStream.read(blocSizeBuffer.data(), 4);



    return true;
}