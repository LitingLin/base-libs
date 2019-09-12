#include <base/mutex.h>

#include <base/logging.h>
#include <base/security_attributes.h>

namespace Base
{
	NamedMutex::NamedMutex(const wchar_t* name, bool acl)
	{
		if (acl)
			_handle = CreateMutex(nullptr, FALSE, name);
		else
		{
			AllowAllSecurityATTRIBUTES security_attributes;
			_handle = CreateMutex(security_attributes.get(), FALSE, name);
		}

		L_ENSURE_WIN32API(_handle);
	}

	NamedMutex::~NamedMutex()
	{
		CloseHandle(_handle);
	}

	void NamedMutex::lock()
	{
		L_ENSURE_EQ_WIN32API(WaitForSingleObject(_handle, INFINITE), WAIT_OBJECT_0);
	}

	bool NamedMutex::try_lock()
	{
		const DWORD rc = WaitForSingleObject(_handle, 0);
		if (rc == WAIT_OBJECT_0)
			return true;
		else if (rc == WAIT_TIMEOUT)
			return false;
		else
			L_UNREACHABLE_ERROR;
		return false;
	}

	bool NamedMutex::unlock()
	{
		return ReleaseMutex(_handle);
	}

	HANDLE NamedMutex::getHandle() const
	{
		return _handle;
	}
}