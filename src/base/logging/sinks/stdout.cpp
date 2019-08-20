#include <base/logging/sinks/stdout.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
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
		void StdOutSink::write(std::string_view message)
		{
			std::wstring UTF16Message = Details::UTF8ToUTF16(message);
			WriteConsoleW()
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