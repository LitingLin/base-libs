#pragma once

#include <base/exception.h>
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
		int64_t _errorCode;
		ErrorCodeType _errorCodeType;
		std::ostringstream _stream;
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
    template <typename Type, template <class Type2 = Type> class Operator>
    struct _ReverseComparator
    {
        _ReverseComparator(const Type& first, const Type& second)
                : first(first), second(second) {}
        operator bool()
        {
            Operator<> op;
            return !op(first, second);
        }
        Type first;
        Type second;
    };
	struct _StreamTypeVoidify
	{
		void operator&(std::ostream&) const {}
	};
}

/* ----------------------------- HELPER MACRO ----------------------------- */
#define _COMPARATOR_NAME2(PREFIX, LINENUMBER) _PP_CAT(PREFIX, LINENUMBER)
#define _COMPARATOR_NAME _COMPARATOR_NAME2(COMPARATOR_, __LINE__)

#define LOG_GET_LEFT_EXPRESSION_RC \
	_values_.first
#define LOG_GET_RIGHT_EXPRESSION_RC \
	_values_.second

/* ----------------------------- GENERIC ----------------------------- */
#define _LOG_GENERIC(loggingClass, errorCodeType, errorCode, handler) \
loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__).stream()

#define _LOG_CONDITIONED_GENERIC(condition, loggingClass, errorCodeType, errorCode, handler) \
(condition) ? (void) 0 : Base::_StreamTypeVoidify() & loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #condition).stream()

#define _LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, op, functional_op, loggingClass, errorCodeType, errorCode, handler) \
if (auto _values_ = Base::_Comparator<decltype(leftExp), functional_op> ((leftExp), (rightExp))) ; else loggingClass(errorCodeType, errorCode, handler, __FILE__, __LINE__, __func__, #leftExp, #op, #rightExp).stream()

#define _LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, ==, std::equal_to, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, != , std::not_equal_to, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, >= , std::greater_equal, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, > , std::greater, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, <= , std::less_equal, loggingClass, errorCodeType, errorCode, handler)

#define _LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, loggingClass, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, < , std::less, loggingClass, errorCodeType, errorCode, handler)


/* ----------------------------- UNRECOVERABLE ERROR ----------------------------- */
#define NOT_IMPLEMENTED_ERROR \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

#define UNREACHABLE_ERROR \
_LOG_GENERIC(Base::FatalErrorLogging, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

/* ----------------------------- FATAL ERROR ----------------------------- */
// "Let it crash"

#define FATAL_ERROR(...) _PP_MACRO_OVERLOAD(_FATAL_ERROR, __VA_ARGS__)
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

#define ENSURE_STDCAPI_WITH_HANDLER(condition, handler) \
_ENSURE_EQ_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)
#define ENSURE_STDCAPI(condition) \
ENSURE_STDCAPI_WITH_HANDLER(condition, nullptr)

#define ENSURE_NE_STDCAPI(condition, value) \
_ENSURE_NE_WITH_HANDLER_5(condition, value, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

/* ----------------------------- UNEXPECTED ERROR ----------------------------- */
// But recoverable
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

#define CHECK_STDCAPI_WITH_HANDLER(condition, handler) \
_CHECK_EQ_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)
#define CHECK_STDCAPI(condition) \
CHECK_STDCAPI_WITH_HANDLER(condition, nullptr)
#define CHECK_NE_STDCAPI_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)
#define CHECK_NE_STDCAPI(leftExp, rightExp) \
CHECK_NE_STDCAPI_WITH_HANDLER(leftExp, rightExp, nullptr)
#define CHECK_EQ_STDCAPI_WITH_HANDLER(leftExp, rightExp, handler) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)
#define CHECK_EQ_STDCAPI(leftExp, rightExp) \
CHECK_EQ_STDCAPI_WITH_HANDLER(leftExp, rightExp, nullptr)


/* ----------------------------- LOGGING ONLY ----------------------------- */
#define LOG_IF_FAILED_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_FAILED_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_FAILED_WITH_HANDLER_4(condition, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_GENERIC(condition, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_FAILED_WITH_HANDLER_3(condition, errorCode, handler) \
_LOG_IF_FAILED_WITH_HANDLER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_FAILED_WITH_HANDLER_2(condition, handler) \
_LOG_IF_FAILED_WITH_HANDLER_3(condition, -1, handler)

#define LOG_IF_FAILED(...) _PP_MACRO_OVERLOAD(_LOG_IF_FAILED, __VA_ARGS__)
#define _LOG_IF_FAILED_3(condition, errorCodeType, errorCode) \
_LOG_IF_FAILED_WITH_HANDLER_4(condition, errorCodeType, errorCode, nullptr)
#define _LOG_IF_FAILED_2(condition, errorCode) \
_LOG_IF_FAILED_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_FAILED_1(condition) \
_LOG_IF_FAILED_2(condition, -1)

#define LOG_IF_NOT_EQ_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_EQ_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_EQ_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_EQ_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_EQ_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_EQ(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_EQ, __VA_ARGS__)
#define _LOG_IF_NOT_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_EQ_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_EQ_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_EQ_2(leftExp, rightExp) \
_LOG_IF_NOT_EQ_3(leftExp, rightExp, -1)

#define LOG_IF_NOT_NE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_NE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_NE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_NE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_NE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_NE(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_NE, __VA_ARGS__)
#define _LOG_IF_NOT_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_NE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_NE_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_NE_2(leftExp, rightExp) \
_LOG_IF_NOT_NE_3(leftExp, rightExp, -1)

#define LOG_IF_NOT_GE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_GE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_GE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_GE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_GE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_GE(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_GE, __VA_ARGS__)
#define _LOG_IF_NOT_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_GE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_GE_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_GE_2(leftExp, rightExp) \
_LOG_IF_NOT_GE_3(leftExp, rightExp, -1)

#define LOG_IF_NOT_GT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_GT_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_GT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_GT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_GT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_GT(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_GT, __VA_ARGS__)
#define _LOG_IF_NOT_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_GT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_GT_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_GT_2(leftExp, rightExp) \
_LOG_IF_NOT_GT_3(leftExp, rightExp, -1)

#define LOG_IF_NOT_LE_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_LE_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_LE_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_LE_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_LE_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_LE(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_LE, __VA_ARGS__)
#define _LOG_IF_NOT_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_LE_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_LE_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_LE_2(leftExp, rightExp) \
_LOG_IF_NOT_LE_3(leftExp, rightExp, -1)

#define LOG_IF_NOT_LT_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_LT_WITH_HANDLER, __VA_ARGS__)
#define _LOG_IF_NOT_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, handler) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, Base::EventLogging, errorCodeType, errorCode, handler)
#define _LOG_IF_NOT_LT_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_LOG_IF_NOT_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, handler)
#define _LOG_IF_NOT_LT_WITH_HANDLER_3(leftExp, rightExp, handler) \
_LOG_IF_NOT_LT_WITH_HANDLER_4(leftExp, rightExp, -1, handler)

#define LOG_IF_NOT_LT(...) _PP_MACRO_OVERLOAD(_LOG_IF_NOT_LT, __VA_ARGS__)
#define _LOG_IF_NOT_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_LOG_IF_NOT_LT_WITH_HANDLER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _LOG_IF_NOT_LT_3(leftExp, rightExp, errorCode) \
_LOG_IF_NOT_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _LOG_IF_NOT_LT_2(leftExp, rightExp) \
_LOG_IF_NOT_LT_3(leftExp, rightExp, -1)

#define LOG_IF_FAILED_STDCAPI_WITH_HANDLER(condition, handler) \
_LOG_IF_FAILED_WITH_HANDLER_4(condition, Base::ErrorCodeType::STDCAPI, errno, handler) << Base::getStdCApiErrorString(errno)

#define LOG_IF_NOT_EQ_STDCAPI(leftExp, rightExp) \
_LOG_IF_NOT_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

#define LOG_IF_NOT_NE_STDCAPI(leftExp, rightExp) \
_LOG_IF_NOT_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)
