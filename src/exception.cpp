#include <base/exception.h>

namespace Base
{
	FatalError::FatalError(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType)
		: _message(message), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	FatalError::FatalError(std::string&& message, int64_t errorCode, ErrorCodeType errorCodeType)
		: _message(std::move(message)), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	int64_t FatalError::getErrorCode() const
	{
		return _errorCode;
	}

	int FatalError::getErrorCodeAsCRTErrno() const
	{
		return (int)_errorCode;
	}

#ifdef _WIN32
	DWORD FatalError::getErrorCodeAsWinAPI() const
	{
		return (DWORD)_errorCode;
	}

	HRESULT FatalError::getErrorCodeAsHRESULT() const
	{
		return (HRESULT)_errorCode;
	}

	NTSTATUS FatalError::getErrorCodeAsNTSTATUS() const
	{
		return (NTSTATUS)_errorCode;
	}
#endif

	ErrorCodeType FatalError::getErrorCodeType() const
	{
		return _errorCodeType;
	}

	char const* FatalError::what() const
	{
		return _message.data();
	}


	RuntimeException::RuntimeException(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType)
		: _message(message), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	RuntimeException::RuntimeException(std::string&& message, int64_t errorCode, ErrorCodeType errorCodeType)
		: _message(std::move(message)), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	int64_t RuntimeException::getErrorCode() const
	{
		return _errorCode;
	}
	
	int RuntimeException::getErrorCodeAsCRTErrno() const
	{
		return (int)_errorCode;
	}

#ifdef _WIN32
	DWORD RuntimeException::getErrorCodeAsWinAPI() const
	{
		return (DWORD)_errorCode;
	}

	HRESULT RuntimeException::getErrorCodeAsHRESULT() const
	{
		return (HRESULT)_errorCode;
	}

	NTSTATUS RuntimeException::getErrorCodeAsNTSTATUS() const
	{
		return (NTSTATUS)_errorCode;
	}
#endif

	ErrorCodeType RuntimeException::getErrorCodeType() const
	{
		return _errorCodeType;
	}

	char const* RuntimeException::what() const
	{
		return _message.data();
	}

#ifdef _WIN32
    HRESULT getHRESULTFromException(const RuntimeException& exp)
	{
		if (exp.getErrorCodeType() == ErrorCodeType::HRESULT)
			return exp.getErrorCodeAsHRESULT();
		else if (exp.getErrorCodeType() == ErrorCodeType::WIN32API)
			return HRESULT_FROM_WIN32(exp.getErrorCodeAsWinAPI());
		else
			return E_FAIL;
	}

	HRESULT getHRESULTFromException(const FatalError& exp)
	{
		if (exp.getErrorCodeType() == ErrorCodeType::HRESULT)
			return exp.getErrorCodeAsHRESULT();
		else if (exp.getErrorCodeType() == ErrorCodeType::WIN32API)
			return HRESULT_FROM_WIN32(exp.getErrorCodeAsWinAPI());
		else
			return E_FAIL;
	}
#endif
}