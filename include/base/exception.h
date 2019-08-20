#pragma once

#include <base/common.h>
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
		GENERIC, STDCAPI, SQLITE3, CUDA,
#ifdef _WIN32
		WIN32API, HRESULT, NTSTATUS,
#endif
	};
		
	class ATTRIBUTE_INTERFACE FatalError : public std::exception
	{
	public:
		FatalError(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType);
		FatalError(std::string&& message, int64_t errorCode, ErrorCodeType errorCodeType);
		int64_t getErrorCode() const;
		int getErrorCodeAsCRTErrno() const;
#ifdef _WIN32
		DWORD getErrorCodeAsWinAPI() const;
		HRESULT getErrorCodeAsHRESULT() const;
		NTSTATUS getErrorCodeAsNTSTATUS() const;
#endif
		ErrorCodeType getErrorCodeType() const;
		~FatalError() noexcept override = default;
		char const* what() const override;
	private:
		std::string _message;
		int64_t _errorCode;
		ErrorCodeType _errorCodeType;
	};

	class ATTRIBUTE_INTERFACE RuntimeException : public std::exception
	{
	public:
		RuntimeException(const std::string& message, int64_t errorCode, ErrorCodeType errorCodeType);
		RuntimeException(std::string&& message, int64_t errorCode, ErrorCodeType errorCodeType);
		int64_t getErrorCode() const;
		int getErrorCodeAsCRTErrno() const;
#ifdef _WIN32
		DWORD getErrorCodeAsWinAPI() const;
		HRESULT getErrorCodeAsHRESULT() const;
		NTSTATUS getErrorCodeAsNTSTATUS() const;
#endif
		ErrorCodeType getErrorCodeType() const;
		~RuntimeException() noexcept override = default;
		char const* what() const override;
	private:
		std::string _message;
		int64_t _errorCode;
		ErrorCodeType _errorCodeType;
	};

#ifdef _WIN32
	ATTRIBUTE_INTERFACE
    HRESULT getHRESULTFromException(const FatalError& exp);
	ATTRIBUTE_INTERFACE
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