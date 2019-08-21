#pragma once

#include <base/common.h>
#include <string>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <bcrypt.h>

namespace Base
{
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

	ATTRIBUTE_INTERFACE
	std::string getWin32LastErrorString();
	ATTRIBUTE_INTERFACE
	std::string getWin32ErrorString(DWORD errorCode);
	ATTRIBUTE_INTERFACE
	std::wstring getHRESULTErrorWString(HRESULT hr);
	ATTRIBUTE_INTERFACE
	std::string getHRESULTErrorString(HRESULT hr);
	ATTRIBUTE_INTERFACE
	std::wstring getNtStatusErrorWString(NTSTATUS ntstatus);
	ATTRIBUTE_INTERFACE
	std::string getNtStatusErrorString(NTSTATUS ntStatus);
	
}
