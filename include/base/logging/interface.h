#pragma once

#include <base/logging/common.h>
#include <base/logging/sinks/interface.h>
#include <string_view>

namespace Base
{
	namespace Logging
	{
		LOGGING_INTERFACE
		int addSink(Sink* sink);
		LOGGING_INTERFACE
		void removeSink(int handle);
		LOGGING_INTERFACE
		void log(std::string_view message);
	};
};
