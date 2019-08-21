#pragma once

#include <vector>
#include <shared_mutex>
#include <functional>
#include <string_view>
#include <memory>

namespace Base
{
	namespace Logging
	{
		class Sink;
		namespace Details
		{
			class LoggingMessageDispatcher
			{
			public:
				int addSink(Sink* sink);
				void write(std::string_view string);
				void removeSink(int handle);
				void flush();
				~LoggingMessageDispatcher();
			private:
				void safe_executor(const std::function<void(const std::unique_ptr<Sink>&)>& function);
				std::shared_mutex _mutex;
				std::vector<std::pair<int, std::unique_ptr<Sink>>> _sinks;
			} extern g_dispatcher ;			
		}
	}
}
