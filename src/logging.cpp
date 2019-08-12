#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#endif
#include <stdarg.h>
#include <string.h>

#include <base/logging.h>

namespace Base
{
	const char *get_base_file_name(const char *file_name)
	{
		const char *base_file_name = file_name;
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
		catch (std::exception & e) {
			logger->warn("Exception occured during executing exception handler. Message:{}", e.what());
		}
		catch (...) {
			logger->warn("Exception occured during executing exception handler.");
		}
	}

	_BaseLogging::_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function)
		: _ExceptionHandlerExecutor(handler), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
		_stream << '[' << get_base_file_name(file) << ':' << line << ' ' << function << "] ";
	}
	_BaseLogging::_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function)
	{
		_stream << "Check failed: " << exp << ' ';
	}
	_BaseLogging::_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function)
	{
		_stream << "Check failed: " << leftExp << ' ' << op << ' ' << rightExp << ' ';
	}

	std::ostringstream& _BaseLogging::stream()
	{
		return _stream;
	}

	FatalErrorLogging::FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function) {}
	FatalErrorLogging::FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, exp) {}
	FatalErrorLogging::FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, leftExp, op, rightExp) {}

	FatalErrorLogging::~FatalErrorLogging() noexcept(false)
	{
		_stream << std::endl
			<< "*** Check failure stack trace: ***" << std::endl
			<< getStackTrace();
		if (std::uncaught_exceptions())
			logger->critical("Fatal error occurred during exception handling. Message: {}", _stream.str());
		else {
			logger->critical(_stream.str());
			throw FatalError(_stream.str(), _errorCode, _errorCodeType);
		}
	}

	RuntimeExceptionLogging::RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function) {}
	RuntimeExceptionLogging::RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, exp) {}
	RuntimeExceptionLogging::RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, leftExp, op, rightExp) {}

	RuntimeExceptionLogging::~RuntimeExceptionLogging() noexcept(false)
	{
		if (std::uncaught_exceptions())
			logger->error("Runtime exception occurred during exception handling. Message: {}", _stream.str());
		else {
			logger->warn(_stream.str());
			throw RuntimeException(_stream.str(), _errorCode, _errorCodeType);
		}
	}
	
	EventLogging::EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function) {}
	EventLogging::EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, exp) {}
	EventLogging::EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
		: _BaseLogging(errorCodeType, errorCode, handler, file, line, function, leftExp, op, rightExp) {}

	EventLogging::~EventLogging() noexcept(false)
	{
		logger->info(_stream.str());
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