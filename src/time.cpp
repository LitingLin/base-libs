#include <base/time.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <spdlog/fmt/fmt.h>

namespace Base
{
	std::wstring getFormatedCurrentTime()
	{
		SYSTEMTIME systemtime;
		GetSystemTime(&systemtime);
		return fmt::format(L"UTC-{}-{}-{}.{}.{}.{}.{}", systemtime.wYear, systemtime.wMonth, systemtime.wDay,
			systemtime.wHour, systemtime.wMinute, systemtime.wSecond, systemtime.wMilliseconds);
	}
}