#include <base/debugoutput_logger_sink.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <base/utils.h>

namespace Base
{
	void debugoutput_sink::sink_it_(const spdlog::details::log_msg& msg)
	{
		OutputDebugString(UTF8ToUTF16(msg.payload.data()).c_str());
	}

	void debugoutput_sink::flush_()
	{
	}
}
