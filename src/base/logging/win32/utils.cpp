#include <base/logging/win32/utils.h>

#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Base {
	namespace Logging {
		namespace Details {
			std::string UTF16ToUTF8(std::wstring_view str)
			{
				if (str.empty())
					return std::string();
				int str_size;
				if (str.size() > size_t(std::numeric_limits<int>::max()))
					str_size = std::numeric_limits<int>::max();
				else
					str_size = int(str.size());
				int size = WideCharToMultiByte(CP_UTF8, 0, str.data(), str_size, nullptr, 0, NULL, NULL);
				std::string localMultiByteString;
				localMultiByteString.resize(size);
				if (WideCharToMultiByte(CP_UTF8, 0, str.data(), str_size, &localMultiByteString[0], size + 1, NULL, NULL) == 0)
					return "WideCharToMultiByte() failed. win32 error code: " + std::to_string(GetLastError());
				return localMultiByteString;
			}

			std::wstring UTF8ToUTF16(std::string_view str)
			{
				if (str.empty())
					return std::wstring();
				int str_size;
				if (str.size() > size_t(std::numeric_limits<int>::max()))
					str_size = std::numeric_limits<int>::max();
				else
					str_size = int(str.size());
				int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), str_size, NULL, NULL);
				std::wstring wideCharString;
				wideCharString.resize(size);
				if (MultiByteToWideChar(CP_UTF8, 0, str.data(), str_size, &wideCharString[0], size + 1) == 0)
					return L"MultiByteToWideChar() failed. win32 error code: " + std::to_wstring(GetLastError());
				return wideCharString;
			}
		}
	}
}
