#pragma once

#include "core/context/Context.h"
#include <array>
#include <fstream>
#include <string>

class WAVAudioFileDecoder
{
private:

    static constexpr std::size_t _RIFF_HEADER_LENGTH = 4;
    static constexpr std::array<char, _RIFF_HEADER_LENGTH> _RIFF_HEADER = {'R', 'I', 'F', 'F'};

    static constexpr std::size_t _WAVE_HEADER_LENGTH = 4;
    static constexpr std::array<char, _WAVE_HEADER_LENGTH> _WAVE_HEADER = {'W', 'A', 'V', 'E'};

    static constexpr std::size_t _FMT_HEADER_LENGTH = 4;
    static constexpr std::array<char, _FMT_HEADER_LENGTH> _FMT_HEADER = {'f', 'm', 't', ' '};


    std::ifstream _inputFileStream;

public:
    bool openFile(std::string filePath);

    char getByte();

    void resetFileStream();

    bool validateHeader();
};