#pragma once

#include <base/logging/sinks/interface.h>

namespace Base
{
	namespace Logging
	{
		class StdOutSink : public Sink
		{
		public:
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;			
		};
	}
}