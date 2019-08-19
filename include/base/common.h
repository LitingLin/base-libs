#pragma once

#if defined _WIN32
#ifdef COMPILING_DLL
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
