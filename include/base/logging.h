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
	std::string getCRTErrorString(int errnum);

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
	template <typename T1, typename T2, typename Operator>
	struct _Comparator
	{
		_Comparator(const T1& first, const T2& second)
			: first(first), second(second) {}
		operator bool()
		{
			return Operator(first, second);
		}
		T1 first;
		T2 second;
	};
	struct _StreamTypeVoidify
	{
		void operator&(std::ostream&) {}
	};
}

#define LEFT_OPERAND_RC \
	_rc_->first
#define RIGHT_OPERAND_RC \
	_rc_->second

#define _LOG_GENERIC(loggingClass, errorCodeType, errorCode, handler) \
loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__).stream()

#define _LOG_CONDITIONED_GENERIC(condition, loggingClass, errorCodeType, errorCode, handler) \
!(condition) ? (void) 0 : _StreamTypeVoidify() & loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #condition).stream()

#define _LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, op, functional_op, loggingClass, errorCodeType, errorCode, handler) \
!(auto _rc_ = Base::check_impl((leftExp), (rightExp), functional_op)) ? (void) 0 : _StreamTypeVoidify() & loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #leftExp, #op, #rightExp).stream()

#define _LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, ==, std::equal_to<>(), loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, != , std::not_equal_to<>(), loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, >= , std::greater_equal<>(), loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, > , std::greater<>(), loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, <= , std::less_equal<>(), loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, < , std::less<>(), loggingClass, errorCodeType, errorCode, handler)

#define FATAL_ERROR(...) _PP_MACRO_OVERLOAD(_FATAL_EEROR, __VA_ARGS__)
#define _FATAL_ERROR_1(errorCode) \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::USERDEFINED, errorCode, nullptr)

// Let it crash, don't catch
#define _FATAL_ERROR_0() \
_FATAL_ERROR_1(-1)

#define FATAL_ERROR_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_FATAL_EEROR_HANDLER, __VA_ARGS__)
#define _FATAL_EEROR_HANDLER_2(errorCode, handler) \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::USERDEFINED, errorCode, handler)
#define _FATAL_EEROR_HANDLER_1(handler) \
_FATAL_EEROR_HANDLER_2(-1, handler)

#define ENSURE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_WITH_HANDLER_4(condition, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_GENERIC(condition, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_WITH_HANDLER_3(condition, errorCode, handler) \
_ENSURE_WITH_HANDLER_4(condition, Base::ErrorCodeType::USERDEFINED, errorCode, handler)
#define _ENSURE_WITH_HANDLER_2(condition, handler) \
_ENSURE_WITH_HANDLER_3(condition, -1, handler)

#define ENSURE(...) _PP_MACRO_OVERLOAD(_ENSURE, __VA_ARGS__)
#define _ENSURE_3(condition, errorCodeType, errorCode) \
_ENSURE_WITH_HANDLER_4(condition, errorCodeType, errorCode, nullptr)
#define _ENSURE_2(condition, errorCode) \
_ENSURE_3(condition, Base::ErrorCodeType::USERDEFINED, errorCode)
#define _ENSURE_1(condition) \
_ENSURE_2(condition, -1)

#define ENSURE_EQ_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_ENSURE_EQ_WITH_HANDLER, __VA_ARGS__)
#define _ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, Base::FatalErrorLogging, errorCodeType, errorCode, handler)
#define _ENSURE_EQ_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::USERDEFINED, errorCode, handler)
#define _ENSURE_EQ_WITH_HANDLER_3(leftExp, rightExp, handler) \
_ENSURE_EQ_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define ENSURE_EQ(...) _PP_MACRO_OVERLOAD(_ENSURE_EQ, __VA_ARGS__)
#define _ENSURE_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_ENSURE_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _ENSURE_EQ_3(leftExp, rightExp, errorCode) \
_ENSURE_EQ_4(leftExp, rightExp, Base::ErrorCodeType::USERDEFINED, errorCode)
#define _ENSURE_EQ_2(leftExp, rightExp) \
_ENSURE_EQ_3(leftExp, rightExp, -1)

#define ENSURE_WIN32API(condition) \
_ENSURE_3(condition, Base::ErrorCodeType::WIN32API, GetLastError()) << Base::getWin32LastErrorString()

#define ENSURE_HR(condition)

#define ENSURE_HR(exp) \
	if (HRESULT _hr = (exp)) if (_hr < 0) \
Base::FatalErrorLogging(Base::ErrorCodeType::HRESULT, _hr, __FILE__, __LINE__, __func__, #exp).stream() << Base::getHRESULTErrorString(_hr)

#define ENSURE_NTSTATUS(val) \
	if (NTSTATUS _status = (val)) if (_status < 0) \
Base::FatalErrorLogging(Base::ErrorCodeType::NTSTATUS, _status, __FILE__, __LINE__, __func__, #val).stream() << Base::getNtStatusErrorString(_status)

#define ENSURE_CRTAPI(val) \
	if (val) \
Base::FatalErrorLogging(Base::ErrorCodeType::CRT, errno, __FILE__, __LINE__, __func__, #val).stream() << Base::getCRTErrorString(errno)

#define ENSURE_OP(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::FatalErrorLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "

#define ENSURE_OP_WITH_CODE(exp1, exp2, op, functional_op, errorcode)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::FatalErrorLogging(Base::ErrorCodeType::USERDEFINED, errorcode, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "

#define ENSURE_OP_WIN32API(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::FatalErrorLogging(Base::ErrorCodeType::WIN32API, GetLastError(), __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getWin32LastErrorString()

#define ENSURE_OP_CRTAPI(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::FatalErrorLogging(Base::ErrorCodeType::CRT, errno, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getCRTErrorString(errno)

#define ENSURE_EQ(exp1, exp2) \
	ENSURE_OP(exp1, exp2, ==, std::equal_to<>())
#define ENSURE_NE(exp1, exp2) \
	ENSURE_OP(exp1, exp2, !=, std::not_equal_to<>())
#define ENSURE_LE(exp1, exp2) \
	ENSURE_OP(exp1, exp2, <=, std::less_equal<>())
#define ENSURE_LT(exp1, exp2) \
	ENSURE_OP(exp1, exp2, <, std::less<>())
#define ENSURE_GE(exp1, exp2) \
	ENSURE_OP(exp1, exp2, >=, std::greater_equal<>())
#define ENSURE_GT(exp1, exp2) \
	ENSURE_OP(exp1, exp2, >, std::greater<>())

#define ENSURE_EQ_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, ==, std::equal_to<>(), errorcode)
#define ENSURE_NE_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, !=, std::not_equal_to<>(), errorcode)
#define ENSURE_LE_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, <=, std::less_equal<>(), errorcode)
#define ENSURE_LT_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, <, std::less<>(), errorcode)
#define ENSURE_GE_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, >=, std::greater_equal<>(), errorcode)
#define ENSURE_GT_WITH_CODE(exp1, exp2, errorcode) \
	ENSURE_OP_WITH_CODE(exp1, exp2, >, std::greater<>(), errorcode)

#define ENSURE_EQ_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, ==, std::equal_to<>())
#define ENSURE_NE_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, !=, std::not_equal_to<>())
#define ENSURE_LE_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, <=, std::less_equal<>())
#define ENSURE_LT_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, <, std::less<>())
#define ENSURE_GE_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, >=, std::greater_equal<>())
#define ENSURE_GT_WIN32API(exp1, exp2) \
	ENSURE_OP_WIN32API(exp1, exp2, >, std::greater<>())

#define ENSURE_EQ_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, ==, std::equal_to<>())
#define ENSURE_NE_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, !=, std::not_equal_to<>())
#define ENSURE_LE_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, <=, std::less_equal<>())
#define ENSURE_LT_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, <, std::less<>())
#define ENSURE_GE_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, >=, std::greater_equal<>())
#define ENSURE_GT_CRTAPI(exp1, exp2) \
	ENSURE_OP_CRTAPI(exp1, exp2, >, std::greater<>())

#define NOT_IMPLEMENTED_ERROR \
	Base::FatalErrorLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__).stream() << "Unknown internal error. "

#define UNREACHABLE_ERROR \
	Base::FatalErrorLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__).stream() << "Unknown internal error. "

// Normal error
#define CHECK(val) \
	if (!(val)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__, #val).stream()

#define CHECK_WITH_CODE(val, errorcode) \
	if (!(val)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::USERDEFINED, errorcode, __FILE__, __LINE__, __func__, #val).stream()

#define CHECK_WIN32API(val) \
	if (!(val)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::WIN32API, GetLastError(), __FILE__, __LINE__, __func__, #val).stream() << Base::getWin32LastErrorString()

#define CHECK_HR(exp) \
	if (HRESULT _hr = (exp)) if (_hr < 0) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::HRESULT, _hr, __FILE__, __LINE__, __func__, #exp).stream() << Base::getHRESULTErrorString(_hr)

#define CHECK_NTSTATUS(val) \
	if (NTSTATUS _status = (val)) if (_status < 0) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::NTSTATUS, _status, __FILE__, __LINE__, __func__, #val).stream() << Base::getNtStatusErrorString(_status)

#define CHECK_CRTAPI(val) \
	if (val) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::CRT, errno, __FILE__, __LINE__, __func__, #val).stream() << Base::getCRTErrorString(errno)

#define CHECK_OP(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "

#define CHECK_OP_WITH_CODE(exp1, exp2, op, functional_op, errorcode)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::USERDEFINED, errorcode, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "

#define CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, op, functional_op, errorcode)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::WIN32API, errorcode, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "  << getWin32ErrorString(errorcode)

#define CHECK_OP_HR(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::HRESULT, LEFT_OPERAND_RC, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "  << Base::getHRESULTErrorString(LEFT_OPERAND_RC)

#define CHECK_OP_WIN32API(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::WIN32API, GetLastError(), __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getWin32LastErrorString()

#define CHECK_OP_CRTAPI(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::RuntimeExceptionLogging(Base::ErrorCodeType::CRT, errno, __FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getCRTErrorString(errno)

#define CHECK_EQ(exp1, exp2) \
	CHECK_OP(exp1, exp2, ==, std::equal_to<>())
#define CHECK_NE(exp1, exp2) \
	CHECK_OP(exp1, exp2, !=, std::not_equal_to<>())
#define CHECK_LE(exp1, exp2) \
	CHECK_OP(exp1, exp2, <=, std::less_equal<>())
#define CHECK_LT(exp1, exp2) \
	CHECK_OP(exp1, exp2, <, std::less<>())
#define CHECK_GE(exp1, exp2) \
	CHECK_OP(exp1, exp2, >=, std::greater_equal<>())
#define CHECK_GT(exp1, exp2) \
	CHECK_OP(exp1, exp2, >, std::greater<>())

#define CHECK_EQ_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, ==, std::equal_to<>(), errorcode)
#define CHECK_NE_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, !=, std::not_equal_to<>(), errorcode)
#define CHECK_LE_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, <=, std::less_equal<>(), errorcode)
#define CHECK_LT_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, <, std::less<>(), errorcode)
#define CHECK_GE_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, >=, std::greater_equal<>(), errorcode)
#define CHECK_GT_WITH_CODE(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE(exp1, exp2, >, std::greater<>(), errorcode)

#define CHECK_EQ_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, ==, std::equal_to<>(), errorcode)
#define CHECK_NE_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, !=, std::not_equal_to<>(), errorcode)
#define CHECK_LE_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, <=, std::less_equal<>(), errorcode)
#define CHECK_LT_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, <, std::less<>(), errorcode)
#define CHECK_GE_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, >=, std::greater_equal<>(), errorcode)
#define CHECK_GT_WITH_CODE_WIN32API(exp1, exp2, errorcode) \
	CHECK_OP_WITH_CODE_WIN32API(exp1, exp2, >, std::greater<>(), errorcode)

#define CHECK_EQ_HR(exp1, exp2) \
	CHECK_OP_HR(exp1, exp2, ==, std::equal_to<>())
#define CHECK_NE_HR(exp1, exp2) \
	CHECK_OP_HR(exp1, exp2, !=, std::not_equal_to<>())

#define CHECK_EQ_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, ==, std::equal_to<>())
#define CHECK_NE_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, !=, std::not_equal_to<>())
#define CHECK_LE_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, <=, std::less_equal<>())
#define CHECK_LT_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, <, std::less<>())
#define CHECK_GE_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, >=, std::greater_equal<>())
#define CHECK_GT_WIN32API(exp1, exp2) \
	CHECK_OP_WIN32API(exp1, exp2, >, std::greater<>())

#define CHECK_EQ_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, ==, std::equal_to<>())
#define CHECK_NE_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, !=, std::not_equal_to<>())
#define CHECK_LE_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, <=, std::less_equal<>())
#define CHECK_LT_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, <, std::less<>())
#define CHECK_GE_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, >=, std::greater_equal<>())
#define CHECK_GT_CRTAPI(exp1, exp2) \
	CHECK_OP_CRTAPI(exp1, exp2, >, std::greater<>())

#define NOT_EXPECT_EXCEPTION \
	Base::RuntimeExceptionLogging(Base::ErrorCodeType::USERDEFINED, -1, __FILE__, __LINE__, __func__).stream() << "Unexpected parameter."

// Just logging
#define LOG_IF_FAILED(val) \
	if (!(val)) \
Base::EventLogging(__FILE__, __LINE__, __func__, #val).stream()

#define LOG_IF_FAILED_WIN32API(val) \
	if (!(val)) \
Base::EventLogging(__FILE__, __LINE__, __func__, #val).stream() << Base::getWin32LastErrorString()

#define LOG_IF_FAILED_HR(exp) \
	if (HRESULT _hr = (exp)) if (_hr < 0) \
Base::EventLogging(__FILE__, __LINE__, __func__, #exp).stream() << Base::getHRESULTErrorString(_hr)

#define LOG_IF_FAILED_NTSTATUS(val) \
	if (NTSTATUS _status = (val)) if (_status < 0) \
Base::EventLogging(__FILE__, __LINE__, __func__, #val).stream() << Base::getNtStatusErrorString(_status)

#define LOG_IF_FAILED_CRTAPI(val) \
	if (val) \
Base::EventLogging(__FILE__, __LINE__, __func__, #val).stream() << Base::getCRTErrorString(errno)

#define LOG_IF_OP(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::EventLogging(__FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") "

#define LOG_IF_OP_WIN32API(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::EventLogging(__FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getWin32LastErrorString()

#define LOG_IF_OP_CRTAPI(exp1, exp2, op, functional_op)  \
	if (auto _rc = Base::check_impl((exp1), (exp2), functional_op)) \
Base::EventLogging(__FILE__, __LINE__, __func__, #exp1, #op, #exp2).stream() \
	<< '(' << LEFT_OPERAND_RC << " vs. " << RIGHT_OPERAND_RC << ") " << Base::getCRTErrorString(errno)

#define LOG_IF_NOT_EQ(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, ==, std::equal_to<>())
#define LOG_IF_NOT_NE(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, !=, std::not_equal_to<>())
#define LOG_IF_NOT_LE(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, <=, std::less_equal<>())
#define LOG_IF_NOT_LT(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, <, std::less<>())
#define LOG_IF_NOT_GE(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, >=, std::greater_equal<>())
#define LOG_IF_NOT_GT(exp1, exp2) \
	LOG_IF_OP(exp1, exp2, >, std::greater<>())

#define LOG_IF_NOT_EQ_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, ==, std::equal_to<>())
#define LOG_IF_NOT_NE_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, !=, std::not_equal_to<>())
#define LOG_IF_NOT_LE_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, <=, std::less_equal<>())
#define LOG_IF_NOT_LT_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, <, std::less<>())
#define LOG_IF_NOT_GE_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, >=, std::greater_equal<>())
#define LOG_IF_NOT_GT_WIN32API(exp1, exp2) \
	LOG_IF_OP_WIN32API(exp1, exp2, >, std::greater<>())

#define LOG_IF_NOT_EQ_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, ==, std::equal_to<>())
#define LOG_IF_NOT_NE_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, !=, std::not_equal_to<>())
#define LOG_IF_NOT_LE_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, <=, std::less_equal<>())
#define LOG_IF_NOT_LT_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, <, std::less<>())
#define LOG_IF_NOT_GE_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, >=, std::greater_equal<>())
#define LOG_IF_NOT_GT_CRTAPI(exp1, exp2) \
	LOG_IF_OP_CRTAPI(exp1, exp2, >, std::greater<>())
