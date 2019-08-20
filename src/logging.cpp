#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#endif
#include <stdarg.h>
#include <string.h>

#include <base/logging.h>
#include <shared_mutex>
#include <string_view>

namespace Base
{
	namespace Logging {
		namespace Detail {
			enum class Action
			{
				THROW_FATAL_ERROR,
				THROW_RUNTIME_EXCEPTION,
				LOGGING_ONLY
			};

			class FatalError : std::exception
			{
			public:
				FatalError(const std::string& message)
					: _message(message) {}
				FatalError(std::string&& message)
					: _message(std::move(message)) {}
				~FatalError() noexcept = default;
				char const* what() const override
				{
					return _message.data();
				}
			private:
				std::string _message;
			};

			class RuntimeException : std::exception
			{
			public:
				RuntimeException(const std::string& message)
					: _message(message) {}
				RuntimeException(std::string&& message)
					: _message(std::move(message)) {}
				~RuntimeException() noexcept = default;
				char const* what() const override
				{
					return _message.data();
				}
			private:
				std::string _message;
			};

			class Sink
			{
			public:
				virtual std::string_view getName() = 0;
				virtual void write(std::string_view message) = 0;
				virtual void flush() = 0;
			};
			
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
					for (auto &_sink : _sinks)
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
				void safe_executor(const std::function<void(const std::unique_ptr<Sink>&)> &function)
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

			struct LoggingMessageFinalHandler
			{
			public:
				void setMessage(const std::string& message)
				{
					_message = message;
				}
				void setMessage(std::string&& message)
				{
					_message = std::move(message);
				}
				void setAction(Action action)
				{
					_action = action;
				}
				~LoggingMessageFinalHandler() noexcept(false)
				{
					if (_action == Action::THROW_FATAL_ERROR)
					{
						throw FatalError(std::move(_message));
					}
					else if (_action == Action::THROW_RUNTIME_EXCEPTION)
					{
						throw RuntimeException(std::move(_message));
					}
				}
			protected:
				std::string _message;
				Action _action;
			};
			
			class FatalErrorLoggingStream : public LoggingMessageFinalHandler
			{
			public:
				std::stringstream& stream()
				{
					return _stream;
				}
				~FatalErrorLoggingStream()
				{
					_stream << std::endl
						<< "*** Check failure stack trace: ***" << std::endl
						<< getStackTrace();
					if (std::uncaught_exceptions()) {
						setAction(Action::LOGGING_ONLY);
						setMessage(fmt::format("Fatal error occurred during exception handling.\n{}", _stream.str());
					}
					else {
						setMessage(_stream.str());
						setAction(Action::THROW_FATAL_ERROR);
					}
				}
			private:
				std::stringstream _stream;
			};

			class RuntimeExceptionLoggingStream : public LoggingMessageFinalHandler
			{
			public:
				std::stringstream& stream()
				{
					return _stream;
				}
				~RuntimeExceptionLoggingStream()
				{
					if (std::uncaught_exceptions()) {
						setAction(Action::LOGGING_ONLY);
						setMessage(fmt::format("Fatal error occurred during exception handling.\n{}", _stream.str());
					}
					else {
						setMessage(_stream.str());
						setAction(Action::THROW_RUNTIME_EXCEPTION);
					}
				}
			private:
				std::stringstream _stream;
			};
			
			const char* get_base_file_name(const char* file_name)
			{
				const char* base_file_name = file_name;
				while (*file_name != '\0') {
					if (*file_name == '\\' || *file_name == '/')
						base_file_name = file_name + 1;
					file_name++;
				}

				return base_file_name;
			}

			_ExceptionHandlerExecutor::_ExceptionHandlerExecutor(std::function<void(void)> function)
			{
				try {
					if (function) function();
				}
				catch (std::exception& e) {
					logger->warn("Exception occured during executing exception handler. Message:{}", e.what());
				}
				catch (...) {
					logger->warn("Exception occured during executing exception handler.");
				}
			}

			std::string generateHeader(const char* file, int line, const char* function)
			{
				return fmt::format("[{}:{} {}] ", file, line, function);
			}

			std::string generateHeader(const char* file, int line, const char* function, const char* exp)
			{
				return fmt::format("[{}:{} {}] Check failed: {} ", file, line, function, exp);
			}

			std::string generateHeader(const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
			{
				return fmt::format("[{}:{} {}] Check failed: {} {} {} ", file, line, function, leftExp, op, rightExp);
			}



#ifdef _WIN32
			std::string getStdCApiErrorString(int errnum)
			{
				char buffer[256];
				std::string errString;
				if (!strerror_s(buffer, errnum))
					return std::string(buffer);
				else
					return std::string();
			}
#else
			std::string getStdCApiErrorString(int errnum)
			{
				return std::string(strerror(errnum));
			}

#endif
			}
		}
	}
