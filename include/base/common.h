#pragma once

#if defined _WIN32
#if defined COMPILING_DLL && defined _WINDLL
#define ATTRIBUTE_INTERFACE __declspec(dllexport)
#else
#define ATTRIBUTE_INTERFACE
#endif
#else
#ifdef COMPILING_DLL
#define ATTRIBUTE_INTERFACE __attribute__ ((visibility ("default")))
#else
#define ATTRIBUTE_INTERFACE
#endif
#endif
