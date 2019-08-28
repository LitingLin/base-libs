#pragma once

#include <string>
#include <string_view>

#ifdef _WIN32
#define PLATFORM_STRING_TYPE std::wstring
#define PLATFORM_STRING_VIEW_TYPE std::wstring_view
#else
#define PLATFORM_STRING_TYPE std::string
#define PLATFORM_STRING_VIEW_TYPE std::string_view
#endif

#ifdef _WIN32
#include <tchar.h>
#else
#define _T(x) x
#endif