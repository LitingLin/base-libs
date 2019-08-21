#include <base/logging/sinks/win32_debugger.h>

#include <base/logging/win32/utils.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Base
{
	namespace Logging
	{
		std::string_view Win32DebuggerSink::getName()
		{
			return "Win32 Debugger";
		}

		void Win32DebuggerSink::write(std::string_view message)
		{
			std::wstring UTF16Message = Details::UTF8ToUTF16(message);
			OutputDebugStringW(UTF16Message.data());
		}

		void Win32DebuggerSink::flush()
		{
		}
	}
}
