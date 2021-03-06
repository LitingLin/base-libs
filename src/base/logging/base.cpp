#include <base/logging/base.h>
#include <base/exception.h>
#include <base/debugging.h>

#include <base/logging/dispatcher/dispatcher.h>

namespace Base
{
	namespace Logging
	{
		namespace Details
		{
			LoggingMessageFinalHandler::LoggingMessageFinalHandler(ErrorCodeType errorCodeType, int64_t errorCode)
			: _errorCodeType(errorCodeType), _errorCode(errorCode)
			{
			}
			void LoggingMessageFinalHandler::setMessage(const std::string& message)
			{
				_message = message;
			}
			void LoggingMessageFinalHandler::setMessage(std::string&& message)
			{
				_message = std::move(message);
			}
			void LoggingMessageFinalHandler::setAction(Action action)
			{
				_action = action;
			}
			LoggingMessageFinalHandler::~LoggingMessageFinalHandler() noexcept(false)
			{
				g_dispatcher.write(_message);
				
				if (_action == Action::THROW_FATAL_ERROR)
				{
					throw FatalError(std::move(_message), _errorCode, _errorCodeType);
				}
				else if (_action == Action::THROW_RUNTIME_EXCEPTION)
				{
					throw RuntimeException(std::move(_message), _errorCode, _errorCodeType);
				}
			}
		}

		FatalErrorLoggingStream::FatalErrorLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode)
			: LoggingMessageFinalHandler(errorCodeType, errorCode) { }

		std::stringstream& FatalErrorLoggingStream::stream()
		{
			return _stream;
		}

		FatalErrorLoggingStream::~FatalErrorLoggingStream()
		{
			_stream << std::endl
				<< "*** Check failure stack trace: ***" << std::endl
				<< getStackTrace();
			if (std::uncaught_exceptions()) {
				setAction(Details::Action::LOGGING_ONLY);
				setMessage("Fatal error occurred during exception handling.\n" + _stream.str());
			}
			else {
				setMessage(_stream.str());
				setAction(Details::Action::THROW_FATAL_ERROR);
			}
		}

		RuntimeExceptionLoggingStream::RuntimeExceptionLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode)
			: LoggingMessageFinalHandler(errorCodeType, errorCode) { }

		std::stringstream& RuntimeExceptionLoggingStream::stream()
		{
			return _stream;
		}
		RuntimeExceptionLoggingStream::~RuntimeExceptionLoggingStream()
		{
			if (std::uncaught_exceptions()) {
				setAction(Details::Action::LOGGING_ONLY);
				setMessage("Fatal error occurred during exception handling.\n" + _stream.str());
			}
			else {
				setMessage(_stream.str());
				setAction(Details::Action::THROW_RUNTIME_EXCEPTION);
			}
		}

		EventLoggingStream::EventLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode)
			: LoggingMessageFinalHandler(errorCodeType, errorCode) { }

		std::stringstream& EventLoggingStream::stream()
		{
			return _stream;
		}

		EventLoggingStream::~EventLoggingStream()
		{
			setAction(Details::Action::LOGGING_ONLY);
			setMessage(_stream.str());			
		}
	}
}