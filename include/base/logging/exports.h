#pragma once

#include <spdlog/spdlog.h>

#include <locale>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

extern std::shared_ptr<spdlog::logger> logger;

namespace Base
{
	namespace Logging
	{
		ATTRIBUTE_INTERFACE
			std::string getStackTrace();

		ATTRIBUTE_INTERFACE
			std::string getStdCApiErrorString(int errnum);


		namespace Detail {
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

			class ATTRIBUTE_INTERFACE FatalErrorLogging : public _BaseLogging
			{
			public:
				FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
				FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
				FatalErrorLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
				~FatalErrorLogging() noexcept(false);
			};

			
			class ATTRIBUTE_INTERFACE RuntimeExceptionLogging : public _BaseLogging
			{
			public:
				RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
				RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
				RuntimeExceptionLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
				~RuntimeExceptionLogging() noexcept(false);
			};

			
			class ATTRIBUTE_INTERFACE EventLogging : public _BaseLogging
			{
			public:
				EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function);
				EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* exp);
				EventLogging(ErrorCodeType errorCodeType, int64_t errorCode, std::function<void(void)> handler, const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp);
				~EventLogging() noexcept(false);
			};

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
		}
	}
}