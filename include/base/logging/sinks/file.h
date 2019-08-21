#pragma once

#include <base/logging/common.h>
#include <base/logging/sinks/interface.h>

namespace Base
{
	namespace Logging {
		class LOGGING_INTERFACE FileSink : public Sink
		{
		public:
			FileSink(std::string_view path);
			FileSink(std::wstring_view path);
			std::string_view getName() override;
			void write(std::string_view message) override;
			void flush() override;
			~FileSink() override;
		private:
			FILE* _file;
		};
	}
}
