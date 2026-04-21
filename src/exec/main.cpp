#include <stdio.h>
#include "lib/audio-library.h"

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

    WAVAudioFileDecoder wav_decodec = WAVAudioFileDecoder();
    wav_decodec.openFile("./sounds/pluck.wav");
    wav_decodec.reset();
    
    MP3AudioFileDecoder mp3_decodec = MP3AudioFileDecoder();
    mp3_decodec.openFile("./sounds/pluck.mp3");
    mp3_decodec.reset();
    
    FLACAudioFileDecoder flac_decodec = FLACAudioFileDecoder();
    flac_decodec.openFile("./sounds/pluck.flac");
    flac_decodec.reset();
    
    
    

    return 0;
}
