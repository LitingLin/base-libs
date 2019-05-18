#pragma once

#include <string>

#ifdef _WIN32
#define PLATFORM_STRING_TYPE std::wstring
#else
#define PLATFORM_STRING_TYPE std::string
#endif