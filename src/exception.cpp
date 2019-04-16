#include <base/exception.h>

namespace Base
{
	FatalError::FatalError(const std::string& _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), errorCode(errorCode), errorCodeType(errorCodeType)
	{
	}

	FatalError::FatalError(const char* _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), errorCode(errorCode), errorCodeType(errorCodeType)
	{
	}

	int64_t FatalError::getErrorCode() const
	{
		return errorCode;
	}

#ifdef WIN32
	DWORD FatalError::getErrorCodeAsWinAPI() const
	{
		return (DWORD)errorCode;
	}

	HRESULT FatalError::getErrorCodeAsHRESULT() const
	{
		return (HRESULT)errorCode;
	}

	NTSTATUS FatalError::getErrorCodeAsNTSTATUS() const
	{
		return (NTSTATUS)errorCode;
	}
#endif

	errno_t FatalError::getErrorCodeAsCRT() const
	{
		return (errno_t)errorCode;
	}

	ErrorCodeType FatalError::getErrorCodeType() const
	{
		return errorCodeType;
	}


	RuntimeException::RuntimeException(const std::string& _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), errorCode(errorCode), errorCodeType(errorCodeType)
	{
	}

	RuntimeException::RuntimeException(const char* _Message, int64_t errorCode, ErrorCodeType errorCodeType)
		: runtime_error(_Message), errorCode(errorCode), errorCodeType(errorCodeType)
	{
	}

	int64_t RuntimeException::getErrorCode() const
	{
		return errorCode;
	}

#ifdef WIN32
	DWORD RuntimeException::getErrorCodeAsWinAPI() const
	{
		return (DWORD)errorCode;
	}

	HRESULT RuntimeException::getErrorCodeAsHRESULT() const
	{
		return (HRESULT)errorCode;
	}

	NTSTATUS RuntimeException::getErrorCodeAsNTSTATUS() const
	{
		return (NTSTATUS)errorCode;
	}
#endif

	errno_t RuntimeException::getErrorCodeAsCRT() const
	{
		return (errno_t)errorCode;
	}

	ErrorCodeType RuntimeException::getErrorCodeType() const
	{
		return errorCodeType;
	}

#ifdef WIN32
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