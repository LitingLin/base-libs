#pragma once

namespace Base {
	namespace Logging {
		class ATTRIBUTE_INTERFACE Win32ErrorCodeToString
		{
		public:
			Win32ErrorCodeToString(unsigned long errorCode, ...);
			std::string getString() const;
			std::wstring getWString() const;
			~Win32ErrorCodeToString();
		private:
			mutable std::string ansi_str;
			wchar_t* str;
		};

		std::string getWin32LastErrorString();
		std::string getWin32ErrorString(DWORD errorCode);
		std::wstring getHRESULTErrorWString(HRESULT hr);
		std::string getHRESULTErrorString(HRESULT hr);
		std::wstring getNtStatusErrorWString(NTSTATUS ntstatus);
		std::string getNtStatusErrorString(NTSTATUS ntStatus);
	}
}

/* ----------------------------- FATAL ERROR ----------------------------- */
// "Let it crash"
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
#define ENSURE_NTSTATUS(condition) \
ENSURE_NTSTATUS_WITH_HANDLER(condition, nullptr)

/* ----------------------------- UNEXPECTED ERROR ----------------------------- */
// But recoverable
#define CHECK_WIN32API_WITH_HANDLER(condition, handler) \
_CHECK_WITH_HANDLER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define CHECK_WIN32API(condition) \
CHECK_WIN32API_WITH_HANDLER(condition, nullptr)
#define CHECK_EQ_WIN32API_WITH_HANDLER(...) _PP_MACRO_OVERLOAD(_CHECK_EQ_WIN32API_WITH_HANDLER, __VA_ARGS__)
#define _CHECK_EQ_WIN32API_WITH_HANDLER_3(leftExp, rightExp, handler) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define _CHECK_EQ_WIN32API_WITH_HANDLER_4(leftExp, rightExp, errorCode, handler) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, errorCode, handler) << Base::getWin32ErrorString(errorCode)
#define CHECK_EQ_WIN32API(...) _PP_MACRO_OVERLOAD(_CHECK_EQ_WIN32API, __VA_ARGS__)
#define _CHECK_EQ_WIN32API_2(leftExp, rightExp) \
_CHECK_EQ_WIN32API_WITH_HANDLER_3(leftExp, rightExp, nullptr)
#define _CHECK_EQ_WIN32API_3(leftExp, rightExp, errorCode) \
_CHECK_EQ_WIN32API_WITH_HANDLER_4(leftExp, rightExp, errorCode, nullptr)
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
#define CHECK_EQ_HR(leftExp, rightExp) \
_CHECK_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, nullptr) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define CHECK_NTSTATUS_WITH_HANDLER(condition, handler) \
_CHECK_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define CHECK_NTSTATUS(condition) \
CHECK_NTSTATUS_WITH_HANDLER(condition, nullptr)


/* ----------------------------- LOGGING ONLY ----------------------------- */
#define LOG_IF_FAILED_WIN32API_WITH_HANDLER(condition, handler) \
_LOG_IF_FAILED_WITH_HANDLER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_FAILED_WIN32API(condition) \
LOG_IF_FAILED_WIN32API_WITH_HANDLER(condition, nullptr)
#define LOG_IF_NOT_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_EQ_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_EQ_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_EQ_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NOT_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_NE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_NE_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_NE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NOT_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_GE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_GE_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_GE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NOT_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_GT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_GT_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_GT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NOT_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_LE_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_LE_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_LE_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)
#define LOG_IF_NOT_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, handler) \
_LOG_IF_NOT_LT_WITH_HANDLER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), handler) << Base::getWin32LastErrorString()
#define LOG_IF_NOT_LT_WIN32API(leftExp, rightExp) \
LOG_IF_NOT_LT_WIN32API_WITH_HANDLER(leftExp, rightExp, nullptr)

#define LOG_IF_FAILED_HR_WITH_HANDLER(condition, handler) \
_LOG_IF_NOT_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define LOG_IF_FAILED_HR(condition) \
LOG_IF_FAILED_HR_WITH_HANDLER(condition, nullptr);
#define LOG_IF_FAILED_NTSTATUS_WITH_HANDLER(condition, handler) \
_LOG_IF_NOT_GE_WITH_HANDLER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, handler) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define LOG_IF_FAILED_NTSTATUS(condition) \
LOG_IF_FAILED_NTSTATUS_WITH_HANDLER(condition, nullptr)