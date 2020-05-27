#pragma once

#include <stdint.h>

typedef void* HANDLE;

namespace Base
{

	class NamedSemaphore
	{
	public:
		NamedSemaphore();
		~NamedSemaphore();
		bool createOrOpen(const wchar_t* name, uint32_t initialValue = 1);
		void open(const wchar_t* name);
		HANDLE getNativeHandle();
		void acquire();
		bool tryAcquire();
		void release();
	private:
		HANDLE _semaphore;
	};
}
