#include <base/named_semaphore.h>


#define NOMINMAX
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include <base/logging.h>

namespace Base
{
	NamedSemaphore::NamedSemaphore()
		: _semaphore(nullptr)
	{
	}

	NamedSemaphore::~NamedSemaphore()
	{
		if (_semaphore)
			L_LOG_IF_FAILED_WIN32API(CloseHandle(_semaphore));
	}

	bool NamedSemaphore::createOrOpen(const wchar_t* name, uint32_t initialValue)
	{
		_semaphore = CreateSemaphore(nullptr, initialValue, std::numeric_limits<long>::max(), name);
		L_CHECK_WIN32API(_semaphore);
		return GetLastError() != ERROR_ALREADY_EXISTS;
	}

	void NamedSemaphore::open(const wchar_t* name)
	{
		_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, name);
		L_CHECK_WIN32API(_semaphore);
	}

	HANDLE NamedSemaphore::getNativeHandle()
	{
		return _semaphore;
	}

	void NamedSemaphore::acquire()
	{
		L_CHECK_EQ_WIN32API(WaitForSingleObject(_semaphore, INFINITE), WAIT_OBJECT_0);
	}

	bool NamedSemaphore::tryAcquire()
	{
		DWORD returnValue = WaitForSingleObject(_semaphore, 0);
		if (returnValue == WAIT_OBJECT_0)
			return true;
		else if (returnValue == WAIT_TIMEOUT)
			return false;
		else
			L_THROW_WIN32_RUNTIME_EXCEPTION << "WaitForSingleObject() failed with return value " << returnValue;
	}

	void NamedSemaphore::release()
	{
		L_CHECK_WIN32API(ReleaseSemaphore(_semaphore, 1, nullptr));
	}
}
