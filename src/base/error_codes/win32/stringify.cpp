#include <base/error_codes/win32/stringify.h>

#include <base/logging/win32/utils.h>
#include <sstream>

namespace Base
{
	Win32ErrorCodeToString::Win32ErrorCodeToString(unsigned long errorCode, ...)
		: str(nullptr)
	{
		unsigned long systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		unsigned long rc;
		va_list args;
		va_start(args, errorCode);
		rc = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errorCode, systemLocale, reinterpret_cast<wchar_t*>(&str), 0, &args);

		if (rc == 0)
		{
			HMODULE hDLL = LoadLibraryEx(L"netmsg.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
			if (hDLL != NULL)
			{
				rc = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
					hDLL, errorCode, systemLocale, reinterpret_cast<wchar_t*>(&str), 0, &args);
				FreeLibrary(hDLL);
			}
		}

		if (rc == 0)
			str = nullptr;

		va_end(args);
	}

	Win32ErrorCodeToString::~Win32ErrorCodeToString()
	{
		if (str)
			LocalFree(str);
	}

	std::string Win32ErrorCodeToString::getString() const
	{
		if (ansi_str.empty())
			if (str)
			{
				ansi_str = Logging::Details::UTF16ToUTF8(str);
			}

		return ansi_str.c_str();
	}

	std::wstring Win32ErrorCodeToString::getWString() const
	{
		return str;
	}

	std::string getWin32LastErrorString()
	{
		std::ostringstream oss;
		DWORD rc = GetLastError();
		oss << "GetLastError() return: " << rc << ", message: " << Win32ErrorCodeToString(rc).getString();
		return oss.str();
	}

	std::string getWin32ErrorString(DWORD errorCode)
	{
		std::ostringstream oss;
		oss << "GetLastError() return: " << errorCode << ", message: " << Win32ErrorCodeToString(errorCode).getString();
		return oss.str();
	}
#include <lmerr.h>
#include <Errors.h>
#pragma comment(lib,"Quartz.lib")
	std::wstring getDShowErrerString(HRESULT errorMsgId)
	{
		TCHAR szErr[MAX_ERROR_TEXT_LEN];
		DWORD res = AMGetErrorText(errorMsgId, szErr, MAX_ERROR_TEXT_LEN);
		if (!res)
			return std::wstring();
		return std::wstring(szErr, res);
	}

	std::wstring getHRESULTErrorWString(HRESULT dwErrorMsgId)
	{
		DWORD ret = 0;        // Temp space to hold a return value.
		HINSTANCE hInst;  // Instance handle for DLL.
		HLOCAL pBuffer = nullptr;   // Buffer to hold the textual error description.

		if (HRESULT_FACILITY(dwErrorMsgId) == FACILITY_MSMQ)
		{ // MSMQ errors only (see winerror.h for facility info).
		  // Load the MSMQ library containing the error message strings.
			hInst = LoadLibrary(TEXT("MQUTIL.DLL"));
			if (hInst != 0)
			{ // hInst not NULL if the library was successfully loaded.
			  // Get the text string for a message definition
				ret = FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | // Function will handle memory allocation.
					FORMAT_MESSAGE_FROM_HMODULE | // Using a module's message table.
					FORMAT_MESSAGE_IGNORE_INSERTS,
					hInst, // Handle to the DLL.
					dwErrorMsgId, // Message identifier.
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language.
					(LPTSTR)& pBuffer, // Buffer that will hold the text string.
					0, // Allocate at least this many chars for pBuffer.
					NULL // No insert values.
				);
			} // hInst not NULL if the library was successfully loaded.

		} // MSMQ errors only.

		else if (dwErrorMsgId >= NERR_BASE && dwErrorMsgId <= MAX_NERR)
		{ // Could be a network error.
		  // Load the library containing network messages.
			hInst = LoadLibrary(TEXT("NETMSG.DLL"));
			if (hInst != 0)
			{ // Not NULL if successfully loaded.
			  // Get a text string for the message definition.
				ret = FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | // The function will allocate memory for the message.
					FORMAT_MESSAGE_FROM_HMODULE | // Message definition is in a module.
					FORMAT_MESSAGE_IGNORE_INSERTS,  // No inserts used.
					hInst, // Handle to the module containing the definition.
					dwErrorMsgId, // Message identifier.
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language.
					(LPTSTR)& pBuffer, // Buffer to hold the text string.
					0, // Smallest size that will be allocated for pBuffer.
					NULL // No inserts.
				);
			} // Not NULL if successfully loaded.

		} // Could be a network error.
		else
		{ // Unknown message source.
		  // Get the message string from the system.
			ret = FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | // The function will allocate space for pBuffer.
				FORMAT_MESSAGE_FROM_SYSTEM | // System wide message.
				FORMAT_MESSAGE_IGNORE_INSERTS, // No inserts.
				NULL, // Message is not in a module.
				dwErrorMsgId, // Message identifier.
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language.
				(LPTSTR)& pBuffer, // Buffer to hold the text string.
				0, // The function will allocate at least this much for pBuffer.
				NULL // No inserts.
			);
		}


		// Display the string.
		std::wstring errMsg;
		if (ret)
		{
			errMsg = (LPTSTR)pBuffer;
		}
		else
		{
			errMsg = getDShowErrerString(dwErrorMsgId);
		}


		// Free the buffer.
		LocalFree(pBuffer);
		return errMsg;
	}

	std::string getHRESULTErrorString(HRESULT dwErrorMsgId)
	{
		return Logging::Details::UTF16ToUTF8(getHRESULTErrorWString(dwErrorMsgId));
	}

	std::wstring getNtStatusErrorWString(NTSTATUS ntstatus)
	{
		unsigned long systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

		wchar_t* str;
		DWORD rc = 0;
		HMODULE hDLL = LoadLibraryEx(L"kernel32.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
		if (hDLL != NULL)
		{
			rc = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
				hDLL, ntstatus, systemLocale, reinterpret_cast<wchar_t*>(&str), 0, nullptr);
			FreeLibrary(hDLL);
			if (rc != 0) {
				std::wstring message = str;
				LocalFree(str);
				return message;
			}
		}
		if (rc == 0)
		{
			hDLL = LoadLibraryEx(L"Bcrypt.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
			if (hDLL != NULL)
			{
				rc = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
					hDLL, ntstatus, systemLocale, reinterpret_cast<wchar_t*>(&str), 0, nullptr);
				FreeLibrary(hDLL);
				if (rc != 0) {
					std::wstring message = str;
					LocalFree(str);
					return message;
				}
			}
		}
		return std::wstring();
	}

	std::string getNtStatusErrorString(NTSTATUS ntstatus)
	{
		return Logging::Details::UTF16ToUTF8(getNtStatusErrorWString(ntstatus));
	}
}
