#pragma once

#include <base/logging/common.h>
#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <bcrypt.h>

namespace Base
{
	class LOGGING_INTERFACE Win32ErrorCodeToString
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

	LOGGING_INTERFACE
	std::string getWin32LastErrorString();
	LOGGING_INTERFACE
	std::string getWin32ErrorString(DWORD errorCode);
	LOGGING_INTERFACE
	std::wstring getHRESULTErrorWString(HRESULT hr);
	LOGGING_INTERFACE
	std::string getHRESULTErrorString(HRESULT hr);
	LOGGING_INTERFACE
	std::wstring getNtStatusErrorWString(NTSTATUS ntstatus);
	LOGGING_INTERFACE
	std::string getNtStatusErrorString(NTSTATUS ntStatus);
	
}
