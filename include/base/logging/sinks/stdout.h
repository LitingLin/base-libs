#pragma once

#include <base/logging/sinks/interface.h>

namespace Base
{
	namespace Logging
	{
#ifdef _WIN32
		class StdOutSink : public Sink
		{
		public:
			StdOutSink();
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;
		};
#else
		class StdOutSink : public Sink
		{
		public:
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;			
		};
#endif
	}
}