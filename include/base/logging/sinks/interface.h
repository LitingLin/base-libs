#pragma once
#include <string_view>

namespace Base
{
	namespace Logging
	{
		class Sink
		{
		public:
			virtual ~Sink() = default;
			virtual std::string_view getName() = 0;
			virtual void write(std::string_view message) = 0;
			virtual void flush() = 0;
		};
	}
}
