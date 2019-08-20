#pragma once

#include <mutex>

namespace Base
{
	namespace Logging
	{
		namespace Details
		{

			class LoggingMessageDispatcher
			{
			public:
				int addSink(Sink* sink)
				{
					std::unique_lock<std::shared_mutex> lockGuard(_mutex);
					int handle;
					if (_sinks.empty())
						handle = 0;
					else
						handle = _sinks.rbegin()->first + 1;
					_sinks.emplace_back(handle, std::unique_ptr<Sink>(sink));
					return handle;
				}
				void write(std::string_view string)
				{
					safe_executor([string](const std::unique_ptr<Sink>& sink)
						{
							sink->write(string);
						});
				}
				void removeSink(int handle)
				{
					std::unique_ptr<Sink> sinkToRemove;
					{
						std::unique_lock<std::shared_mutex> lockGuard(_mutex);
						for (size_t j = 0; j < _sinks.size(); ++j)
						{
							if ((std::ptrdiff_t)_sinks[j].first == handle)
							{
								std::swap(sinkToRemove, _sinks[j].second);
								_sinks.erase(_sinks.begin() + j);
								break;
							}
						}
					}
				}
				void flush()
				{
					safe_executor([](const std::unique_ptr<Sink>& sink)
						{
							sink->flush();
						});
				}
				~LoggingMessageDispatcher()
				{
					for (auto& _sink : _sinks)
					{
						auto& sink = _sink.second;
						try
						{
							sink->flush();
						}
						catch (...) {}
					}
				}
			private:
				void safe_executor(const std::function<void(const std::unique_ptr<Sink>&)>& function)
				{
					std::vector<std::ptrdiff_t> failedSinks;
					std::vector<std::string> failedSinkMessages;
					{
						std::shared_lock<std::shared_mutex> lockGuard(_mutex);
						for (auto& _sink : _sinks)
						{
							auto& sink = _sink.second;
							try
							{
								function(sink);
							}
							catch (std::exception& exception)
							{
								failedSinks.push_back((std::ptrdiff_t)sink.get());
								failedSinkMessages.push_back(fmt::format("Exception thrown when writing to sink {}, message: {}\n", sink->getName(), exception.what()));
							}
							catch (...)
							{
								failedSinks.push_back((std::ptrdiff_t)sink.get());
								failedSinkMessages.push_back(fmt::format("Exception thrown when writing to sink {}\n", sink->getName()));
							}
						}
					}
					if (!failedSinks.empty())
					{
						std::vector<std::unique_ptr<Sink>> sinksToRemove;
						sinksToRemove.reserve(failedSinks.size());
						std::string errorMessage;
						{
							std::unique_lock<std::shared_mutex> lockGuard(_mutex);
							for (auto failedSinkAddress : failedSinks)
							{
								for (size_t j = 0; j < _sinks.size(); ++j)
								{
									if ((std::ptrdiff_t)_sinks[j].second.get() == failedSinkAddress)
									{
										sinksToRemove.push_back(std::unique_ptr<Sink>());
										_sinks[j].second.swap(*sinksToRemove.rbegin());
										_sinks.erase(_sinks.begin() + j);
										break;
									}
								}
							}
						}
						for (auto& exceptionMessage : failedSinkMessages)
						{
							errorMessage += exceptionMessage;
						}
						for (auto& sink : sinksToRemove)
						{
							errorMessage += fmt::format("Sink {} deleted from logging system.\n", sink->getName());
						}
						if (!errorMessage.empty())
							write(errorMessage);
					}
				}
				std::shared_mutex _mutex;
				std::vector<std::pair<int, std::unique_ptr<Sink>>> _sinks;
			};

		}
	}
}
