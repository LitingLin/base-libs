#pragma once

#if defined _WIN32
#if defined COMPILING_LOGGING && defined _WINDLL
#define LOGGING_INTERFACE __declspec(dllexport)
#else
#define LOGGING_INTERFACE
#endif
#else
#ifdef COMPILING_LOGGING
#define LOGGING_INTERFACE __attribute__ ((visibility ("default")))
#else
#define LOGGING_INTERFACE
#endif
#endif
