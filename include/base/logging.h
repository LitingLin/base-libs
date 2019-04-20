#pragma once

#include "exception.h"
#include "utils.h"
#include <base/preprocessor.h>

#include <locale>
#include <functional>
#include <codecvt>
#include <memory>
#include <sstream>

#ifdef _WIN32
#define SPDLOG_WCHAR_FILENAMES
#endif
#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> logger;
namespace Base
{
	std::string getStackTrace();

#ifdef WIN32
	class Win32ErrorCodeToString
	{
	public:
		Win32ErrorCodeToString(unsigned long errorCode, ...);
		std::string getString() const;
		std::wstring getWString() const;
		~Win32ErrorCodeToString();
	private:
		mutable std::string ansi_str;
		wchar_t *str;
	};

	std::string getWin32LastErrorString();
	std::string getWin32ErrorString(DWORD errorCode);
	std::wstring getHRESULTErrorWString(HRESULT hr);
	std::string getHRESULTErrorString(HRESULT hr);
	std::wstring getNtStatusErrorWString(NTSTATUS ntstatus);
	std::string getNtStatusErrorString(NTSTATUS ntStatus);
#endif
	std::string getStdCApiErrorString(int errnum);

	class _ExceptionHandlerExecutor
	{
	public:
		_ExceptionHandlerExecutor(std::function<void(void)> function);
	};

	class _BaseLogging : public _ExceptionHandlerExecutor
	{
	public:
		_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
		_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
		_BaseLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
		std::ostringstream& stream();
	protected:
		int64_t errorcode;
		ErrorCodeType errorCodeType;
		std::ostringstream str_stream;
	};

	class FatalErrorLogging : public _BaseLogging
	{
	public:
		FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
		FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
		FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
		~FatalErrorLogging() noexcept(false);
	};

	class RuntimeExceptionLogging : public _BaseLogging
	{
	public:
		RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
		RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
		RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
		~RuntimeExceptionLogging() noexcept(false);
	};

	class EventLogging : public _BaseLogging
	{
	public:
		EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
		EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
		EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
		~EventLogging() noexcept(false);
	};

	template <typename T1, typename T2, typename Op>
	std::unique_ptr<std::pair<T1, T2>> check_impl(const T1 &a, const T2 &b, Op op) {
		if (op(a, b))
			return nullptr;
		else
			return std::make_unique<std::pair<T1, T2>>(a, b);
	}
	template <typename Type, template <class Type2 = Type> class Operator>
	struct _Comparator
	{
		_Comparator(const Type& first, const Type& second)
			: first(first), second(second) {}
		operator bool()
		{
			Operator<> op;
			return op(first, second);
		}
		Type first;
		Type second;
	};
	struct _StreamTypeVoidify
	{
		void operator&(std::ostream&) const {}
	};
}

#define LOG_GET_LEFT_EXPRESSION_RC \
	_rc_.first
#define LOG_GET_RIGHT_EXPRESSION_RC \
	_rc_.second

#define _LOG_GENERIC(loggingClass, errorCodeType, errorCode, handler) \
loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__).stream()

#define _LOG_CONDITIONED_GENERIC(condition, loggingClass, errorCodeType, errorCode, handler) \
!(condition) ? (void) 0 : _StreamTypeVoidify() & loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #condition).stream()

#define _LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, op, functional_op, loggingClass, errorCodeType, errorCode, handler) \
(auto _rc_ = _Comparator<decltype(leftExp), functional_op>((leftExp), (rightExp))) ? (void) 0 : _StreamTypeVoidify() & loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #leftExp, #op, #rightExp).stream()

#define _LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, ==, std::equal_to<>, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, != , std::not_equal_to, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, >= , std::greater_equal<>, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, > , std::greater<>, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, <= , std::less_equal<>, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, < , std::less<>, loggingClass, errorCodeType, errorCode, handler)

#define NOT_IMPLEMENTED_ERROR \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

#define UNREACHABLE_ERROR \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

#define FATAL_ERROR(...) _PP_MACRO_OVERLOAD(_FATAL_EEROR, __VA_ARGS__)
#define _FATAL_ERROR_1(errorCode) \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, errorCode, nullptr)

// Let it crash, don't catch
#define _FATAL_ERROR_0() \
_FATAL_ERROR_1(-1)

#define FATAL_ERROR_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_FATAL_ERROR_HANDLER, __VA_ARGS__)
#define _FATAL_ERROR_HANDLER_2(errorCode, handler) \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _FATAL_ERROR_HANDLER_1(handler) \
_FATAL_EEROR_HANDLER_2(-1, handler)

#define ENSURE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_WITH_HANDLER_4(condition, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_GENERIC(condition, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_WITH_HANDLER_3(condition, errorCode, handler) \
_ENSURE_WITH_HANDLER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_WITH_HANDLER_2(condition, handler) \
_ENSURE_WITH_HANDLER_3(condition, -1, handler)

#define ENSURE(...) _PP_MACRO_OVERLOAD(_ENSURE, __VA_ARGS__)
#define _ENSURE_3(condition, errorCodeType, errorCode) \
_ENSURE_WITH_HANDLER_4(condition, errorCodeType, errorCode, nullptr)
#define _ENSURE_2(condition, errorCode) \
_ENSURE_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_1(condition) \
_ENSURE_2(condition, -1)

#define ENSURE_EQ_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_EQ_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_EQ_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_EQ_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_EQ_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_EQ(...) _PP_MACRO_OVERLOAD(_ENSURE_EQ, __VA_ARGS__)
#define _ENSURE_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_EQ_3(leftExp, rightExp, errorCode) \
_ENSURE_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_EQ_2(leftExp, rightExp) \
_ENSURE_EQ_3(leftExp, rightExp, -1)

#define ENSURE_NE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_NE_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_NE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_NE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_NE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_NE(...) _PP_MACRO_OVERLOAD(_ENSURE_NE, __VA_ARGS__)
#define _ENSURE_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_NE_3(leftExp, rightExp, errorCode) \
_ENSURE_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_NE_2(leftExp, rightExp) \
_ENSURE_NE_3(leftExp, rightExp, -1)

#define ENSURE_GE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_GE_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_GE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_GE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_GE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_GE(...) _PP_MACRO_OVERLOAD(_ENSURE_GE, __VA_ARGS__)
#define _ENSURE_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_GE_3(leftExp, rightExp, errorCode) \
_ENSURE_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_GE_2(leftExp, rightExp) \
_ENSURE_GE_3(leftExp, rightExp, -1)

#define ENSURE_GT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_GT_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_GT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_GT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_GT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_GT(...) _PP_MACRO_OVERLOAD(_ENSURE_GT, __VA_ARGS__)
#define _ENSURE_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_GT_3(leftExp, rightExp, errorCode) \
_ENSURE_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_GT_2(leftExp, rightExp) \
_ENSURE_GT_3(leftExp, rightExp, -1)

#define ENSURE_LE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_LE_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_LE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_LE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_LE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_LE(...) _PP_MACRO_OVERLOAD(_ENSURE_LE, __VA_ARGS__)
#define _ENSURE_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_LE_3(leftExp, rightExp, errorCode) \
_ENSURE_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_LE_2(leftExp, rightExp) \
_ENSURE_LE_3(leftExp, rightExp, -1)

#define ENSURE_LT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_LT_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_LT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _ENSURE_LT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_LT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_LT(...) _PP_MACRO_OVERLOAD(_ENSURE_LT, __VA_ARGS__)
#define _ENSURE_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_LT_3(leftExp, rightExp, errorCode) \
_ENSURE_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _ENSURE_LT_2(leftExp, rightExp) \
_ENSURE_LT_3(leftExp, rightExp, -1)

#define ENSURE_WIN32API_WITH_HANDLER(condition, handler) \
_ENSURE_WITH_HANDLER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_WIN32API(condition) \
ENSURE_WIN32API_WITH_HANDLER(condition, nullptr)
#define ENSURE_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_EQ_WIN32API(leftExp, rightExp) \
ENSURE_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define ENSURE_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_NE_WIN32API(leftExp, rightExp) \
ENSURE_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define ENSURE_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_GE_WIN32API(leftExp, rightExp) \
ENSURE_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define ENSURE_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_GT_WIN32API(leftExp, rightExp) \
ENSURE_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define ENSURE_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_LE_WIN32API(leftExp, rightExp) \
ENSURE_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define ENSURE_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_ENSURE_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define ENSURE_LT_WIN32API(leftExp, rightExp) \
ENSURE_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)

#define ENSURE_HR_WITH_HANDLER(condition, handler) \
_ENSURE_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define ENSURE_HR(condition) \
ENSURE_HR_WITH_HANDLER(condition, nullptr);
#define ENSURE_NTSTATUS_WITH_HANDLER(condition, handler) \
_ENSURE_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define ENSURE_NTSTATUS(condition, handler) \
ENSURE_NTSTATUS_WITH_HANDLER(condition, nullptr)
#define ENSURE_STDCAPI_WITH_HANDLER(condition, handler) \
_ENSURE_EQ_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)

#define CHECK_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_WITH_HANDLER_4(condition, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_GENERIC(condition, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_WITH_HANDLER_3(condition, errorCode, handler) \
_CHECK_WITH_HANDLER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_WITH_HANDLER_2(condition, handler) \
_CHECK_WITH_HANDLER_3(condition, -1, handler)

#define CHECK(...) _PP_MACRO_OVERLOAD(_CHECK, __VA_ARGS__)
#define _CHECK_3(condition, errorCodeType, errorCode) \
_CHECK_WITH_HANDLER_4(condition, errorCodeType, errorCode, nullptr)
#define _CHECK_2(condition, errorCode) \
_CHECK_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_1(condition) \
_CHECK_2(condition, -1)

#define CHECK_EQ_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_EQ_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_EQ_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_EQ_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_EQ_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_EQ(...) _PP_MACRO_OVERLOAD(_CHECK_EQ, __VA_ARGS__)
#define _CHECK_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_EQ_3(leftExp, rightExp, errorCode) \
_CHECK_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_EQ_2(leftExp, rightExp) \
_CHECK_EQ_3(leftExp, rightExp, -1)

#define CHECK_NE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_NE_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_NE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_NE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_NE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_NE(...) _PP_MACRO_OVERLOAD(_CHECK_NE, __VA_ARGS__)
#define _CHECK_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_NE_3(leftExp, rightExp, errorCode) \
_CHECK_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_NE_2(leftExp, rightExp) \
_CHECK_NE_3(leftExp, rightExp, -1)

#define CHECK_GE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_GE_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_GE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_GE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_GE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_GE(...) _PP_MACRO_OVERLOAD(_CHECK_GE, __VA_ARGS__)
#define _CHECK_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_GE_3(leftExp, rightExp, errorCode) \
_CHECK_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_GE_2(leftExp, rightExp) \
_CHECK_GE_3(leftExp, rightExp, -1)

#define CHECK_GT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_GT_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_GT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_GT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_GT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_GT(...) _PP_MACRO_OVERLOAD(_CHECK_GT, __VA_ARGS__)
#define _CHECK_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_GT_3(leftExp, rightExp, errorCode) \
_CHECK_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_GT_2(leftExp, rightExp) \
_CHECK_GT_3(leftExp, rightExp, -1)

#define CHECK_LE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_LE_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_LE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_LE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_LE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_LE(...) _PP_MACRO_OVERLOAD(_CHECK_LE, __VA_ARGS__)
#define _CHECK_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_LE_3(leftExp, rightExp, errorCode) \
_CHECK_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_LE_2(leftExp, rightExp) \
_CHECK_LE_3(leftExp, rightExp, -1)

#define CHECK_LT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_LT_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, Base::RuntimeExceptionLogging, errorCodeType, errorCode, handler)
#define _CHECK_LT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _CHECK_LT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_LT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define CHECK_LT(...) _PP_MACRO_OVERLOAD(_CHECK_LT, __VA_ARGS__)
#define _CHECK_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_CHECK_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _CHECK_LT_3(leftExp, rightExp, errorCode) \
_CHECK_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _CHECK_LT_2(leftExp, rightExp) \
_CHECK_LT_3(leftExp, rightExp, -1)

#define CHECK_WIN32API_WITH_HANDLER(condition, handler) \
_CHECK_WITH_HANDLER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_WIN32API(condition) \
CHECK_WIN32API_WITH_HANDLER(condition, nullptr)
#define CHECK_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_EQ_WIN32API(leftExp, rightExp) \
CHECK_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_NE_WIN32API(leftExp, rightExp) \
CHECK_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_GE_WIN32API(leftExp, rightExp) \
CHECK_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_GT_WIN32API(leftExp, rightExp) \
CHECK_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_LE_WIN32API(leftExp, rightExp) \
CHECK_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_LT_WIN32API(leftExp, rightExp) \
CHECK_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)

#define CHECK_HR_WITH_HANDLER(condition, handler) \
_CHECK_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define CHECK_HR(condition) \
CHECK_HR_WITH_HANDLER(condition, nullptr);
#define CHECK_NTSTATUS_WITH_HANDLER(condition, handler) \
_CHECK_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define CHECK_NTSTATUS(condition, handler) \
CHECK_NTSTATUS_WITH_HANDLER(condition, nullptr)
#define CHECK_STDCAPI_WITH_HANDLER(condition, handler) \
_CHECK_EQ_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)

#define LOG_IF_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_WITH_HANDLER_4(condition, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_GENERIC(condition, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_WITH_HANDLER_3(condition, errorCode, handler) \
_LOG_IF_WITH_HANDLER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_WITH_HANDLER_2(condition, handler) \
_LOG_IF_WITH_HANDLER_3(condition, -1, handler)

#define LOG_IF(...) _PP_MACRO_OVERLOAD(_LOG_IF, __VA_ARGS__)
#define _LOG_IF_3(condition, errorCodeType, errorCode) \
_LOG_IF_WITH_HANDLER_4(condition, errorCodeType, errorCode, nullptr)
#define _LOG_IF_2(condition, errorCode) \
_LOG_IF_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_1(condition) \
_LOG_IF_2(condition, -1)

#define LOG_IF_EQ_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_EQ_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_EQ_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_EQ_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_EQ_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_EQ(...) _PP_MACRO_OVERLOAD(_LOG_IF_EQ, __VA_ARGS__)
#define _LOG_IF_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_EQ_3(leftExp, rightExp, errorCode) \
_LOG_IF_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_EQ_2(leftExp, rightExp) \
_LOG_IF_EQ_3(leftExp, rightExp, -1)

#define LOG_IF_NE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NE(...) _PP_MACRO_OVERLOAD(_LOG_IF_NE, __VA_ARGS__)
#define _LOG_IF_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NE_3(leftExp, rightExp, errorCode) \
_LOG_IF_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NE_2(leftExp, rightExp) \
_LOG_IF_NE_3(leftExp, rightExp, -1)

#define LOG_IF_GE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_GE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_GE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_GE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_GE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_GE(...) _PP_MACRO_OVERLOAD(_LOG_IF_GE, __VA_ARGS__)
#define _LOG_IF_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_GE_3(leftExp, rightExp, errorCode) \
_LOG_IF_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_GE_2(leftExp, rightExp) \
_LOG_IF_GE_3(leftExp, rightExp, -1)

#define LOG_IF_GT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_GT_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_GT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_GT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_GT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_GT(...) _PP_MACRO_OVERLOAD(_LOG_IF_GT, __VA_ARGS__)
#define _LOG_IF_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_GT_3(leftExp, rightExp, errorCode) \
_LOG_IF_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_GT_2(leftExp, rightExp) \
_LOG_IF_GT_3(leftExp, rightExp, -1)

#define LOG_IF_LE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_LE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_LE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_LE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_LE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_LE(...) _PP_MACRO_OVERLOAD(_LOG_IF_LE, __VA_ARGS__)
#define _LOG_IF_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_LE_3(leftExp, rightExp, errorCode) \
_LOG_IF_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_LE_2(leftExp, rightExp) \
_LOG_IF_LE_3(leftExp, rightExp, -1)

#define LOG_IF_LT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_LT_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_LT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_LT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_LT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_LT(...) _PP_MACRO_OVERLOAD(_LOG_IF_LT, __VA_ARGS__)
#define _LOG_IF_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_LT_3(leftExp, rightExp, errorCode) \
_LOG_IF_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_LT_2(leftExp, rightExp) \
_LOG_IF_LT_3(leftExp, rightExp, -1)

#define LOG_IF_WIN32API_WITH_HANDLER(condition, handler) \
_LOG_IF_WITH_HANDLER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_WIN32API(condition) \
LOG_IF_WIN32API_WITH_HANDLER(condition, nullptr)
#define LOG_IF_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_EQ_WIN32API(leftExp, rightExp) \
LOG_IF_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NE_WIN32API(leftExp, rightExp) \
LOG_IF_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_GE_WIN32API(leftExp, rightExp) \
LOG_IF_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_GT_WIN32API(leftExp, rightExp) \
LOG_IF_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_LE_WIN32API(leftExp, rightExp) \
LOG_IF_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_LT_WIN32API(leftExp, rightExp) \
LOG_IF_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)

#define LOG_IF_HR_WITH_HANDLER(condition, handler) \
_LOG_IF_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define LOG_IF_HR(condition) \
LOG_IF_HR_WITH_HANDLER(condition, nullptr);
#define LOG_IF_NTSTATUS_WITH_HANDLER(condition, handler) \
_LOG_IF_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define LOG_IF_NTSTATUS(condition, handler) \
LOG_IF_NTSTATUS_WITH_HANDLER(condition, nullptr)
#define LOG_IF_STDCAPI_WITH_HANDLER(condition, handler) \
_LOG_IF_EQ_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)
