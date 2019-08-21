#pragma once

#include <base/common.h>
#include <string_view>

namespace Base
{
	namespace Logging
	{
		class Sink;
		ATTRIBUTE_INTERFACE
		int AddSink(Sink* sink);
		ATTRIBUTE_INTERFACE
		void removeSink(int handle);
		ATTRIBUTE_INTERFACE
		void log(std::string_view message);
	};
};
