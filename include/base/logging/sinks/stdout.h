#pragma once

#include <base/logging/common.h>
#include <base/logging/sinks/interface.h>

namespace Base
{
	namespace Logging
	{
#ifdef _WIN32
		class LOGGING_INTERFACE StdOutSink : public Sink
		{
		public:
			StdOutSink();
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;
		};
#else
		class LOGGING_INTERFACE StdOutSink : public Sink
		{
		public:
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;			
		};
#endif
	}
}