#include <stdio.h>
#include <iostream>

#include "../lib/audio-library.h"

#define PROJECT_NAME "audio-library"

using namespace std;

int main(int argc, char **argv) {
    if (argc != 1) {
        printf("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    printf("This is project %s.\n", PROJECT_NAME);

    Context sample_context(3);

    cout << "Context usage: " << sample_context.get_number() << "\n\n";

    return 0;
}
