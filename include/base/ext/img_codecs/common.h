#pragma once

#if defined _WIN32
#if defined COMPILING_IMAGE_CODECS && defined _WINDLL
#define IMAGE_CODECS_INTERFACE __declspec(dllexport)
#else
#define IMAGE_CODECS_INTERFACE
#endif
#else
#ifdef COMPILING_IMAGE_CODECS
#define IMAGE_CODECS_INTERFACE __attribute__ ((visibility ("default")))
#else
#define IMAGE_CODECS_INTERFACE
#endif
#endif
