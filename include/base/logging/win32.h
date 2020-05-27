#pragma once

#include <base/error_codes/win32/stringify.h>

/* ----------------------------- FATAL ERROR ----------------------------- */
// "Let it crash"
#define L_ENSURE_WIN32API_WITH_FINALIZER(condition, finalizer) \
_L_ENSURE_WITH_FINALIZER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_WIN32API(condition) \
L_ENSURE_WIN32API_WITH_FINALIZER(condition, nullptr)
#define L_ENSURE_EQ_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_EQ_WIN32API(leftExp, rightExp) \
L_ENSURE_EQ_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_ENSURE_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_NE_WIN32API(leftExp, rightExp) \
L_ENSURE_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_ENSURE_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_GE_WIN32API(leftExp, rightExp) \
L_ENSURE_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_ENSURE_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_GT_WIN32API(leftExp, rightExp) \
L_ENSURE_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_ENSURE_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_LE_WIN32API(leftExp, rightExp) \
L_ENSURE_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_ENSURE_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_ENSURE_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_ENSURE_LT_WIN32API(leftExp, rightExp) \
L_ENSURE_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)

#define L_ENSURE_HR_WITH_FINALIZER(condition, finalizer) \
_L_ENSURE_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_ENSURE_HR(condition) \
L_ENSURE_HR_WITH_FINALIZER(condition, nullptr);
#define L_ENSURE_NTSTATUS_WITH_FINALIZER(condition, finalizer) \
_L_ENSURE_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_ENSURE_NTSTATUS(condition) \
L_ENSURE_NTSTATUS_WITH_FINALIZER(condition, nullptr)

/* ----------------------------- UNEXPECTED ERROR ----------------------------- */
// But recoverable

#define L_THROW_WIN32_RUNTIME_EXCEPTION \
_LOG_GENERIC(_LOG_RUNTIME_EXCEPTION_LOGGING_STREAM_CLASS, Base::ErrorCodeType::WIN32API, GetLastError(), nullptr) << Base::getWin32LastErrorString()

#define L_CHECK_WIN32API_WITH_FINALIZER(condition, finalizer) \
_L_CHECK_WITH_FINALIZER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()

#define L_CHECK_WIN32API(condition) \
L_CHECK_WIN32API_WITH_FINALIZER(condition, nullptr)

#define L_CHECK_EQ_WIN32API_WITH_FINALIZER(...) _PP_MACRO_OVERLOAD(_L_CHECK_EQ_WIN32API_WITH_FINALIZER, __VA_ARGS__)
#define _L_CHECK_EQ_WIN32API_WITH_FINALIZER_3(leftExp, rightExp, finalizer) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define _L_CHECK_EQ_WIN32API_WITH_FINALIZER_4(leftExp, rightExp, errorCode, finalizer) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, errorCode, finalizer) << Base::getWin32ErrorString(errorCode)

#define L_CHECK_EQ_WIN32API(...) _PP_MACRO_OVERLOAD(_L_CHECK_EQ_WIN32API, __VA_ARGS__)
#define _L_CHECK_EQ_WIN32API_2(leftExp, rightExp) \
_L_CHECK_EQ_WIN32API_WITH_FINALIZER_3(leftExp, rightExp, nullptr)
#define _L_CHECK_EQ_WIN32API_3(leftExp, rightExp, errorCode) \
_L_CHECK_EQ_WIN32API_WITH_FINALIZER_4(leftExp, rightExp, errorCode, nullptr)

#define L_CHECK_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()

#define L_CHECK_NE_WIN32API(leftExp, rightExp) \
L_CHECK_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_CHECK_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_CHECK_GE_WIN32API(leftExp, rightExp) \
L_CHECK_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_CHECK_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_CHECK_GT_WIN32API(leftExp, rightExp) \
L_CHECK_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_CHECK_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_CHECK_LE_WIN32API(leftExp, rightExp) \
L_CHECK_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_CHECK_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_CHECK_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_CHECK_LT_WIN32API(leftExp, rightExp) \
L_CHECK_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)

#define L_CHECK_HR_WITH_FINALIZER(condition, finalizer) \
_L_CHECK_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_CHECK_HR(condition) \
L_CHECK_HR_WITH_FINALIZER(condition, nullptr);
#define L_CHECK_EQ_HR(leftExp, rightExp) \
_L_CHECK_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, nullptr) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_CHECK_NTSTATUS_WITH_FINALIZER(condition, finalizer) \
_L_CHECK_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_CHECK_NTSTATUS(condition) \
L_CHECK_NTSTATUS_WITH_FINALIZER(condition, nullptr)


/* ----------------------------- LOGGING ONLY ----------------------------- */
#define L_LOG_IF_FAILED_WIN32API_WITH_FINALIZER(condition, finalizer) \
_L_LOG_IF_FAILED_WITH_FINALIZER_4(condition, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()

#define L_LOG_IF_FAILED_WIN32API(condition) \
L_LOG_IF_FAILED_WIN32API_WITH_FINALIZER(condition, nullptr)
#define L_LOG_IF_NOT_EQ_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_EQ_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_EQ_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_EQ_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_LOG_IF_NOT_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_NE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_NE_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_NE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_LOG_IF_NOT_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_GE_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_GE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_LOG_IF_NOT_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_GT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_GT_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_GT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_LOG_IF_NOT_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_LE_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_LE_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_LE_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)
#define L_LOG_IF_NOT_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, finalizer) \
_L_LOG_IF_NOT_LT_WITH_FINALIZER_5(leftExp, rightExp, Base::ErrorCodeType::WIN32API, GetLastError(), finalizer) << Base::getWin32LastErrorString()
#define L_LOG_IF_NOT_LT_WIN32API(leftExp, rightExp) \
L_LOG_IF_NOT_LT_WIN32API_WITH_FINALIZER(leftExp, rightExp, nullptr)

#define L_LOG_IF_FAILED_HR_WITH_FINALIZER(condition, finalizer) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::HRESULT, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getHRESULTErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_LOG_IF_FAILED_HR(condition) \
L_LOG_IF_FAILED_HR_WITH_FINALIZER(condition, nullptr);
#define L_LOG_IF_FAILED_NTSTATUS_WITH_FINALIZER(condition, finalizer) \
_L_LOG_IF_NOT_GE_WITH_FINALIZER_5(condition, 0, Base::ErrorCodeType::NTSTATUS, LOG_GET_LEFT_EXPRESSION_RC, finalizer) << Base::getNtStatusErrorString(LOG_GET_LEFT_EXPRESSION_RC)
#define L_LOG_IF_FAILED_NTSTATUS(condition) \
L_LOG_IF_FAILED_NTSTATUS_WITH_FINALIZER(condition, nullptr)