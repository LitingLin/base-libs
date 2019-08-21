#pragma once

#include <base/logging/common.h>
#include <base/logging/sinks/interface.h>

namespace Base
{
	namespace Logging {
		class LOGGING_INTERFACE Win32DebuggerSink : public Sink
		{
		public:
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;
		};
	}
}