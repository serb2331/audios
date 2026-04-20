#include "core/context/Context.h"
#include "core/file-processing/WAVAudioFileDecoder.h"
#include <iostream>
#include <memory>
#include <ostream>
#include <stdio.h>
#include <memory>

#define PROJECT_NAME "audio-library"

static void setupProject()
{
    Context::GetInstance().setLogging(true);
}

int main(int argc, char **argv) {
    if (argc != 1) {
        printf("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    printf("\n\n\nThis is project %s.\n", PROJECT_NAME);

    setupProject();

    WAVAudioFileDecoder decodec = WAVAudioFileDecoder();
    
    // decodec.testDRLIB("/home/serb/Documents/Licenta/wavs/pluck_test_16bitPCM.wav");

    return 0;
}
