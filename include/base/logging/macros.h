#pragma once

#include <base/logging/common.h>
#include <memory>
#include <string>
#include <functional>
#include <base/logging/base.h>

namespace Base {
	namespace Logging {
		namespace Details {
			template <typename T1, typename T2, typename Op>
			std::unique_ptr<std::pair<T1, T2>> check_impl(const T1& a, const T2& b, Op op) {
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
			
			LOGGING_INTERFACE
			std::string generateHeader(const char* file, int line, const char* function);
			LOGGING_INTERFACE
			std::string generateHeader(const char* file, int line, const char* function, const char* exp);
			LOGGING_INTERFACE
			std::string generateHeader(const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);

			template <typename LoggingStream, class ...Args>
			struct Finalizer : public LoggingStream
			{
			public:
				Finalizer(std::function<void(void)> function, Args... args)
					: LoggingStream(args...) {
					if (function)
						function();
				}
			};

			typedef Finalizer<FatalErrorLoggingStream, ErrorCodeType, int64_t> FatalErrorLoggingStreamWithFinalizer;
			typedef Finalizer<RuntimeExceptionLoggingStream, ErrorCodeType, int64_t> RuntimeExceptionLoggingStreamWithFinalizer;
			typedef Finalizer<EventLoggingStream, ErrorCodeType, int64_t> EventLoggingStreamWithFinalizer;
			
			template<typename S, typename T, typename = void>
			struct is_to_stream_writable : std::false_type {};

			template<typename S, typename T>
			struct is_to_stream_writable<S, T,
				std::void_t<  decltype(std::declval<S&>() << std::declval<T>())  > >
				: std::true_type {};

			template <typename T,
				typename = std::enable_if_t<
				is_to_stream_writable<std::ostream, T>::value>>
				const T& make_stream_writable(const T& v)
			{
				return v;
			}

			template <typename T,
				typename = std::enable_if_t<!
				is_to_stream_writable<std::ostream, T>::value>>
				std::string make_stream_writable(const T& v)
			{
				return "(unstringifiable)";
			}
		}
	}
}

/* ----------------------------- HELPER MACRO ----------------------------- */
#define _COMPARATOR_NAME2(PREFIX, LINENUMBER) _PP_CAT(PREFIX, LINENUMBER)
#define _COMPARATOR_NAME _COMPARATOR_NAME2(COMPARATOR_, __LINE__)

#define LOG_GET_LEFT_EXPRESSION_RC \
	_values_.first
#define LOG_GET_RIGHT_EXPRESSION_RC \
	_values_.second

#define _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS Base::Logging::Details::FatalErrorLoggingStreamWithFinalizer
#define _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS Base::Logging::Details::RuntimeExceptionLoggingStreamWithFinalizer
#define _LOG_EVENT_LOGGING_STREAM_CLASS Base::Logging::Details::EventLoggingStreamWithFinalizer

#define _LOG_IMPL_NAMESPACE Base::Logging::Details

/* ----------------------------- GENERIC ----------------------------- */
#define _LOG_GENERIC(loggingClass, errorCodeType, errorCode, handler) \
loggingClass(handler, errorCodeType, errorCode).stream() << _LOG_IMPL_NAMESPACE::generateHeader(__FILE__, __LINE__, __func__)

#define _LOG_CONDITIONED_GENERIC(condition, loggingClass, errorCodeType, errorCode, handler) \
(condition) ? (void) 0 : _LOG_IMPL_NAMESPACE::_StreamTypeVoidify() & loggingClass(handler, errorCodeType, errorCode).stream() << _LOG_IMPL_NAMESPACE::generateHeader(__FILE__, __LINE__, __func__, #condition)

#define _LOG_CONDITIONED_BINARY_OP_GENERIC(leftExp, rightExp, op, functional_op, loggingClass, errorCodeType, errorCode, handler) \
if (auto _values_ = _LOG_IMPL_NAMESPACE::_Comparator<decltype(leftExp), functional_op> ((leftExp), (rightExp))) ; else loggingClass(handler, errorCodeType, errorCode).stream() << _LOG_IMPL_NAMESPACE::generateHeader(__FILE__, __LINE__, __func__, #leftExp, #op, #rightExp) << "(" << _LOG_IMPL_NAMESPACE::make_stream_writable(LOG_GET_LEFT_EXPRESSION_RC) << " vs. " << _LOG_IMPL_NAMESPACE::make_stream_writable(LOG_GET_RIGHT_EXPRESSION_RC) << ") "

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
#define L_NOT_IMPLEMENTED_ERROR \
_LOG_GENERIC(_LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

#define L_UNREACHABLE_ERROR \
_LOG_GENERIC(_LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, -1, nullptr) << "Unknown internal error. "

/* ----------------------------- FATAL ERROR ----------------------------- */
// "Let it crash"

#define L_FATAL_ERROR(...) _PP_MACRO_OVERLOAD(_L_FATAL_ERROR, __VA_ARGS__)
#define _L_FATAL_ERROR_1(errorCode) \
_LOG_GENERIC(_LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, errorCode, nullptr)

// Let it crash, don't catch
#define _L_FATAL_ERROR_0() \
_L_FATAL_ERROR_1(-1)

#define L_FATAL_ERROR_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_FATAL_ERROR_WITH_FINALIZER, __VA_ARGS__)
#define _L_FATAL_ERROR_WITH_FINALIZER_2(errorCode, finalizer) \
_LOG_GENERIC(_LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_FATAL_ERROR_WITH_FINALIZER_1(finalizer) \
_L_FATAL_ERROR_WITH_FINALIZER_2(-1, finalizer)


#define L_ENSURE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_WITH_FINALIZER_4(condition, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_GENERIC(condition, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_WITH_FINALIZER_3(condition, errorCode, finalizer) \
_L_ENSURE_WITH_FINALIZER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_WITH_FINALIZER_2(condition, finalizer) \
_L_ENSURE_WITH_FINALIZER_3(condition, -1, finalizer)

#define L_ENSURE(...) _PP_MACRO_OVERLOAD(_L_ENSURE, __VA_ARGS__)
#define _L_ENSURE_3(condition, errorCodeType, errorCode) \
_L_ENSURE_WITH_FINALIZER_4(condition, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_2(condition, errorCode) \
_L_ENSURE_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_1(condition) \
_L_ENSURE_2(condition, -1)

#define L_ENSURE_EQ_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_EQ_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_EQ_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_EQ_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_EQ_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_EQ(...) _PP_MACRO_OVERLOAD(_L_ENSURE_EQ, __VA_ARGS__)
#define _L_ENSURE_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_EQ_3(leftExp, rightExp, errorCode) \
_L_ENSURE_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_EQ_2(leftExp, rightExp) \
_L_ENSURE_EQ_3(leftExp, rightExp, -1)

#define L_ENSURE_NE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_NE_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_NE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_NE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_NE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_NE(...) _PP_MACRO_OVERLOAD(_L_ENSURE_NE, __VA_ARGS__)
#define _L_ENSURE_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_NE_3(leftExp, rightExp, errorCode) \
_L_ENSURE_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_NE_2(leftExp, rightExp) \
_L_ENSURE_NE_3(leftExp, rightExp, -1)

#define L_ENSURE_GE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_GE_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_GE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_GE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_GE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_GE(...) _PP_MACRO_OVERLOAD(_L_ENSURE_GE, __VA_ARGS__)
#define _L_ENSURE_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_GE_3(leftExp, rightExp, errorCode) \
_L_ENSURE_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_GE_2(leftExp, rightExp) \
_L_ENSURE_GE_3(leftExp, rightExp, -1)

#define ENSURE_GT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_GT_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_GT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_GT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_GT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_GT(...) _PP_MACRO_OVERLOAD(_L_ENSURE_GT, __VA_ARGS__)
#define _L_ENSURE_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_GT_3(leftExp, rightExp, errorCode) \
_L_ENSURE_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_GT_2(leftExp, rightExp) \
_L_ENSURE_GT_3(leftExp, rightExp, -1)

#define L_ENSURE_LE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_LE_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_LE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_LE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_LE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_LE(...) _PP_MACRO_OVERLOAD(_L_ENSURE_LE, __VA_ARGS__)
#define _L_ENSURE_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_LE_3(leftExp, rightExp, errorCode) \
_L_ENSURE_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_LE_2(leftExp, rightExp) \
_L_ENSURE_LE_3(leftExp, rightExp, -1)

#define L_ENSURE_LT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_ENSURE_LT_WITH_FINALIZER, __VA_ARGS__)
#define _L_ENSURE_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, _LOG_FATAL_ERROR_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_ENSURE_LT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_ENSURE_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_ENSURE_LT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_ENSURE_LT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_ENSURE_LT(...) _PP_MACRO_OVERLOAD(_L_ENSURE_LT, __VA_ARGS__)
#define _L_ENSURE_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_ENSURE_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_ENSURE_LT_3(leftExp, rightExp, errorCode) \
_L_ENSURE_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_ENSURE_LT_2(leftExp, rightExp) \
_L_ENSURE_LT_3(leftExp, rightExp, -1)

#define L_ENSURE_STDCAPI_WITH_FINALIZER(condition, finalizer) \
_L_ENSURE_EQ_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::STDCAPI, errno, finalizer) << Base::getStdCApiErrorString(errno)
#define ENSURE_STDCAPI(condition) \
L_ENSURE_STDCAPI_WITH_FINALIZER(condition, nullptr)

#define L_ENSURE_NE_STDCAPI(condition, value) \
_L_ENSURE_NE_WITH_FINALIZER_5(condition, value, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

/* ----------------------------- UNEXPECTED ERROR ----------------------------- */
// But recoverable
#define L_CHECK_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_WITH_FINALIZER_4(condition, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_GENERIC(condition, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_WITH_FINALIZER_3(condition, errorCode, finalizer) \
_L_CHECK_WITH_FINALIZER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_WITH_FINALIZER_2(condition, finalizer) \
_L_CHECK_WITH_FINALIZER_3(condition, -1, finalizer)

#define L_CHECK(...) _PP_MACRO_OVERLOAD(_L_CHECK, __VA_ARGS__)
#define _L_CHECK_3(condition, errorCodeType, errorCode) \
_L_CHECK_WITH_FINALIZER_4(condition, errorCodeType, errorCode, nullptr)
#define _L_CHECK_2(condition, errorCode) \
_L_CHECK_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_1(condition) \
_L_CHECK_2(condition, -1)

#define L_CHECK_EQ_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_EQ_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_EQ_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_EQ_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_EQ_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_EQ(...) _PP_MACRO_OVERLOAD(_L_CHECK_EQ, __VA_ARGS__)
#define _L_CHECK_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_EQ_3(leftExp, rightExp, errorCode) \
_L_CHECK_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_EQ_2(leftExp, rightExp) \
_L_CHECK_EQ_3(leftExp, rightExp, -1)

#define L_CHECK_NE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_NE_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_NE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_NE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_NE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_NE(...) _PP_MACRO_OVERLOAD(_L_CHECK_NE, __VA_ARGS__)
#define _L_CHECK_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_NE_3(leftExp, rightExp, errorCode) \
_L_CHECK_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_NE_2(leftExp, rightExp) \
_L_CHECK_NE_3(leftExp, rightExp, -1)

#define L_CHECK_GE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_GE_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_GE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_GE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_GE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_GE(...) _PP_MACRO_OVERLOAD(_L_CHECK_GE, __VA_ARGS__)
#define _L_CHECK_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_GE_3(leftExp, rightExp, errorCode) \
_L_CHECK_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_GE_2(leftExp, rightExp) \
_L_CHECK_GE_3(leftExp, rightExp, -1)

#define L_CHECK_GT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_GT_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_GT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_GT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_GT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_GT(...) _PP_MACRO_OVERLOAD(_L_CHECK_GT, __VA_ARGS__)
#define _L_CHECK_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_GT_3(leftExp, rightExp, errorCode) \
_L_CHECK_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_GT_2(leftExp, rightExp) \
_L_CHECK_GT_3(leftExp, rightExp, -1)

#define L_CHECK_LE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_LE_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_LE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_LE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_LE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_LE(...) _PP_MACRO_OVERLOAD(_L_CHECK_LE, __VA_ARGS__)
#define _L_CHECK_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_LE_3(leftExp, rightExp, errorCode) \
_L_CHECK_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_LE_2(leftExp, rightExp) \
_L_CHECK_LE_3(leftExp, rightExp, -1)

#define L_CHECK_LT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_LT_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, _LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_CHECK_LT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_CHECK_LT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_LT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_CHECK_LT(...) _PP_MACRO_OVERLOAD(_L_CHECK_LT, __VA_ARGS__)
#define _L_CHECK_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_CHECK_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_CHECK_LT_3(leftExp, rightExp, errorCode) \
_L_CHECK_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_CHECK_LT_2(leftExp, rightExp) \
_L_CHECK_LT_3(leftExp, rightExp, -1)

#define L_CHECK_STDCAPI_WITH_FINALIZER(condition, finalizer) \
_L_CHECK_WITH_FINALIZER_4(condition, Base::ErrorCodeType::STDCAPI, errno, finalizer) << Base::getStdCApiErrorString(errno)
#define L_CHECK_STDCAPI(condition) \
L_CHECK_STDCAPI_WITH_FINALIZER(condition, nullptr)
#define L_CHECK_NE_STDCAPI_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, finalizer) << Base::getStdCApiErrorString(errno)
#define L_CHECK_NE_STDCAPI(leftExp, rightExp) \
L_CHECK_NE_STDCAPI_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_CHECK_EQ_STDCAPI_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, finalizer) << Base::getStdCApiErrorString(errno)
#define L_CHECK_EQ_STDCAPI(leftExp, rightExp) \
L_CHECK_EQ_STDCAPI_WITH_FINALIZER(leftExp, rightExp, nullptr)

#define L_THROW_RUNTIME_EXCEPTION \
_LOG_GENERIC(_LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, -1, nullptr)

#define L_THROW_STDCAPI_RUNTIME_EXCEPTION \
_LOG_GENERIC(_LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

/* ----------------------------- LOGGING ONLY ----------------------------- */
#define L_LOG_IF_FAILED_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_FAILED_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_FAILED_WITH_FINALIZER_4(condition, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_GENERIC(condition, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_FAILED_WITH_FINALIZER_3(condition, errorCode, finalizer) \
_L_LOG_IF_FAILED_WITH_FINALIZER_4(condition, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_FAILED_WITH_FINALIZER_2(condition, finalizer) \
_L_LOG_IF_FAILED_WITH_FINALIZER_3(condition, -1, finalizer)

#define L_LOG_IF_FAILED(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_FAILED, __VA_ARGS__)
#define _L_LOG_IF_FAILED_3(condition, errorCodeType, errorCode) \
_L_LOG_IF_FAILED_WITH_FINALIZER_4(condition, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_FAILED_2(condition, errorCode) \
_L_LOG_IF_FAILED_3(condition, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_FAILED_1(condition) \
_L_LOG_IF_FAILED_2(condition, -1)

#define L_LOG_IF_NOT_EQ_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_EQ_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_EQ_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_EQ_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_EQ_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_EQ_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_EQ(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_EQ, __VA_ARGS__)
#define _L_LOG_IF_NOT_EQ_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_EQ_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_EQ_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_EQ_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_EQ_2(leftExp, rightExp) \
_L_LOG_IF_NOT_EQ_3(leftExp, rightExp, -1)

#define L_LOG_IF_NOT_NE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_NE_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_NE_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_NE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_NE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_NE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_NE(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_NE, __VA_ARGS__)
#define _L_LOG_IF_NOT_NE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_NE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_NE_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_NE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_NE_2(leftExp, rightExp) \
_L_LOG_IF_NOT_NE_3(leftExp, rightExp, -1)

#define L_LOG_IF_NOT_GE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_GE_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GE_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_GE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_GE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_GE(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_GE, __VA_ARGS__)
#define _L_LOG_IF_NOT_GE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_GE_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_GE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_GE_2(leftExp, rightExp) \
_L_LOG_IF_NOT_GE_3(leftExp, rightExp, -1)

#define L_LOG_IF_NOT_GT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_GT_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_GT_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_GT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_GT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_GT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_GT(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_GT, __VA_ARGS__)
#define _L_LOG_IF_NOT_GT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_GT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_GT_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_GT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_GT_2(leftExp, rightExp) \
_L_LOG_IF_NOT_GT_3(leftExp, rightExp, -1)

#define L_LOG_IF_NOT_LE_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_LE_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LE_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_LE_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_LE_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_LE_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_LE(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_LE, __VA_ARGS__)
#define _L_LOG_IF_NOT_LE_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_LE_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_LE_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_LE_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_LE_2(leftExp, rightExp) \
_L_LOG_IF_NOT_LE_3(leftExp, rightExp, -1)

#define L_LOG_IF_NOT_LT_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_LT_WITH_FINALIZER, __VA_ARGS__)
#define _L_LOG_IF_NOT_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, finalizer) \
_LOG_CONDITIONED_BINARY_OP_LT_GENERIC(leftExp, rightExp, _LOG_EVENT_LOGGING_STREAM_CLASS, errorCodeType, errorCode, finalizer)
#define _L_LOG_IF_NOT_LT_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_LOG_IF_NOT_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode, finalizer)
#define _L_LOG_IF_NOT_LT_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_LT_WITH_FINALIZER_4(leftExp, rightExp, -1, finalizer)

#define L_LOG_IF_NOT_LT(...) _PP_MACRO_OVERLOAD(_L_LOG_IF_NOT_LT, __VA_ARGS__)
#define _L_LOG_IF_NOT_LT_4(leftExp, rightExp, errorCodeType, errorCode) \
_L_LOG_IF_NOT_LT_WITH_FINALIZER_5(leftExp, rightExp, errorCodeType, errorCode, nullptr)
#define _L_LOG_IF_NOT_LT_3(leftExp, rightExp, errorCode) \
_L_LOG_IF_NOT_LT_4(leftExp, rightExp, Base::ErrorCodeType::GENERIC, errorCode)
#define _L_LOG_IF_NOT_LT_2(leftExp, rightExp) \
_L_LOG_IF_NOT_LT_3(leftExp, rightExp, -1)

#define L_LOG_IF_FAILED_STDCAPI_WITH_FINALIZER(condition, finalizer) \
_L_LOG_IF_FAILED_WITH_FINALIZER_4(condition, Base::ErrorCodeType::STDCAPI, errno, finalizer) << Base::getStdCApiErrorString(errno)

#define L_LOG_IF_NOT_EQ_STDCAPI(leftExp, rightExp) \
_L_LOG_IF_NOT_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

#define L_LOG_IF_NOT_NE_STDCAPI(leftExp, rightExp) \
_L_LOG_IF_NOT_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)

#define L_LOG_ERROR \
_LOG_GENERIC(_LOG_EVENT_LOGGING_STREAM_CLASS, Base::ErrorCodeType::GENERIC, -1, nullptr)

#define L_LOG_STDCAPI_ERROR \
_LOG_GENERIC(_LOG_EVENT_LOGGING_STREAM_CLASS, Base::ErrorCodeType::STDCAPI, errno, nullptr) << Base::getStdCApiErrorString(errno)
