#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(AUDIOS_BUILD_LIBRARY)
#define AUDIOS_EXPORT __declspec(dllexport)
#else
#define AUDIOS_EXPORT __declspec(dllimport)
#endif
#else
#if defined(AUDIOS_BUILD_LIBRARY)
#define AUDIOS_EXPORT __attribute__((visibility("default")))
#else
#define AUDIOS_EXPORT
#endif
#endif