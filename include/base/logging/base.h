#pragma once

#include <string>
#include <sstream>
#include <base/logging/common.h>
#include <base/error_codes.h>

namespace Base
{
	namespace Logging
	{
		namespace Details
		{
			enum class Action
			{
				THROW_FATAL_ERROR,
				THROW_RUNTIME_EXCEPTION,
				LOGGING_ONLY
			};
			
			struct LoggingMessageFinalHandler
			{
			public:
				LoggingMessageFinalHandler(ErrorCodeType errorCodeType, int64_t errorCode);
				void setMessage(const std::string& message);
				void setMessage(std::string&& message);
				void setAction(Action action);
				~LoggingMessageFinalHandler() noexcept(false);
			protected:
				std::string _message;
				Action _action;
				ErrorCodeType _errorCodeType;
				int64_t _errorCode;
			};

		}

		class LOGGING_INTERFACE FatalErrorLoggingStream : public Details::LoggingMessageFinalHandler
		{
		public:
			FatalErrorLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode);
			std::stringstream& stream();
			~FatalErrorLoggingStream();
		private:
			std::stringstream _stream;
		};

		class LOGGING_INTERFACE RuntimeExceptionLoggingStream : public Details::LoggingMessageFinalHandler
		{
		public:
			RuntimeExceptionLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode);
			std::stringstream& stream();
			~RuntimeExceptionLoggingStream();
		private:
			std::stringstream _stream;
		};
		
		class LOGGING_INTERFACE EventLoggingStream : public Details::LoggingMessageFinalHandler
		{
		public:
			EventLoggingStream(ErrorCodeType errorCodeType, int64_t errorCode);
			std::stringstream& stream();
			~EventLoggingStream();
		private:
			std::stringstream _stream;
		};
	}
}
