#include <base/exception.h>

namespace Base
{
	FatalError::FatalError(const std::string& _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	FatalError::FatalError(const char* _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), _errorCode(errorCode), _errorCodeType(errorCodeType)
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


	RuntimeException::RuntimeException(const std::string& _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), _errorCode(errorCode), _errorCodeType(errorCodeType)
	{
	}

	RuntimeException::RuntimeException(const char* _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), _errorCode(errorCode), _errorCodeType(errorCodeType)
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