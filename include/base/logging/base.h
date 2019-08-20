#pragma once

#include <string>
#include <sstream>
#include <base/common.h>
#include <base/exception.h>

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
				LoggingMessageFinalHandler(int64_t errorCode, ErrorCodeType errorCodeType);
				void setMessage(const std::string& message);
				void setMessage(std::string&& message);
				void setAction(Action action);
				~LoggingMessageFinalHandler() noexcept(false);
			protected:
				std::string _message;
				Action _action;
				int64_t _errorCode;
				ErrorCodeType _errorCodeType;
			};

		}

		class ATTRIBUTE_INTERFACE FatalErrorLoggingStream : public Details::LoggingMessageFinalHandler
		{
		public:
			FatalErrorLoggingStream(int64_t errorCode, ErrorCodeType errorCodeType);
			std::stringstream& stream();
			~FatalErrorLoggingStream();
		private:
			std::stringstream _stream;
		};

		class ATTRIBUTE_INTERFACE RuntimeExceptionLoggingStream : public Details::LoggingMessageFinalHandler
		{
		public:
			RuntimeExceptionLoggingStream(int64_t errorCode, ErrorCodeType errorCodeType);
			std::stringstream& stream();
			~RuntimeExceptionLoggingStream();
		private:
			std::stringstream _stream;
		};
	}
}
