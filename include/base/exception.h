#pragma once
#include <stdexcept>
#include <stddef.h>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#endif

namespace Base {
	enum class ErrorCodeType
	{
		WIN32API, HRESULT, NTSTATUS, CRT, SQLITE3, CUDA, USERDEFINED
	};

	class FatalError : public std::runtime_error
	{
	public:
		explicit FatalError(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType);
		explicit FatalError(const char* message, int64_t errorCode, ErrorCodeType errorCodeType);
		int64_t getErrorCode() const;
		int getErrorCodeAsCRTErrno() const;
#ifdef _WIN32
		DWORD getErrorCodeAsWinAPI() const;
		HRESULT getErrorCodeAsHRESULT() const;
		NTSTATUS getErrorCodeAsNTSTATUS() const;
#endif
		ErrorCodeType getErrorCodeType() const;
	private:
		int64_t _errorCode;
		ErrorCodeType _errorCodeType;
	};

	class RuntimeException : public std::runtime_error
	{
	public:
		explicit RuntimeException(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType);
		explicit RuntimeException(const char* message, int64_t errorCode, ErrorCodeType errorCodeType);
		int64_t getErrorCode() const;
		int getErrorCodeAsCRTErrno() const;
#ifdef _WIN32
		DWORD getErrorCodeAsWinAPI() const;
		HRESULT getErrorCodeAsHRESULT() const;
		NTSTATUS getErrorCodeAsNTSTATUS() const;
#endif
		ErrorCodeType getErrorCodeType() const;
	private:
		int64_t _errorCode;
		ErrorCodeType _errorCodeType;
	};

#ifdef _WIN32
    HRESULT getHRESULTFromException(const FatalError& exp);
	HRESULT getHRESULTFromException(const RuntimeException& exp);
#endif
}

#ifdef _WIN32
#define RETURN_HRESULT_ON_CAUGHT_EXCEPTION_BEGIN \
try \
{

#define RETURN_HRESULT_ON_CAUGHT_EXCEPTION_END \
} \
catch (Base::RuntimeException &exp) \
{ \
	return Base::getHRESULTFromExceptionType(exp); \
} \
catch (Base::FatalError &exp) \
{ \
return Base::getHRESULTFromExceptionType(exp); \
} \
catch (std::bad_alloc &) \
{ \
return E_OUTOFMEMORY; \
} \
catch (...) \
{ \
return E_FAIL; \
}
#endif