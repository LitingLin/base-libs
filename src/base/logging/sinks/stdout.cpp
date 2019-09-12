#include <base/logging/sinks/stdout.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <cstdio>
#endif

#include <base/logging/win32/utils.h>

namespace Base
{
	namespace Logging
	{
		std::string_view StdOutSink::getName()
		{
			return "stdout";
		}

#ifdef _WIN32
		static HANDLE _stdhandle = nullptr;
		
		StdOutSink::StdOutSink()
		{
			_stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		
		void StdOutSink::write(std::string_view message)
		{
			if (_stdhandle == nullptr || _stdhandle == INVALID_HANDLE_VALUE)
				return;
			std::wstring UTF16Message = Details::UTF8ToUTF16(message);
			DWORD numberOfCharsWritten;
			WriteConsoleW(_stdhandle, UTF16Message.data(), DWORD(UTF16Message.size()), &numberOfCharsWritten, NULL);
		}

		void StdOutSink::flush()
		{
		}		
#else
		void StdOutSink::write(std::string_view message)
		{
			fwrite(message.data(), 1, message.size(), stdout);
		}

		void StdOutSink::flush()
		{
			fflush(stdout);
		}
#endif
	}
}