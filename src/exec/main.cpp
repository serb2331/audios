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
    printf("This is project %s.\n", PROJECT_NAME);

    setupProject();

    WAVAudioFileDecoder decodec = WAVAudioFileDecoder();

    decodec.openFile("sounds/pluck_test_16bitPCM.wav");
    decodec.validateHeader();
    
    return 0;
}
