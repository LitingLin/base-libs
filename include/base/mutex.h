#pragma once

#include <base/common.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Base
{
	class ATTRIBUTE_INTERFACE NamedMutex
	{
	public:
		NamedMutex(const wchar_t *name, bool acl = false);
		~NamedMutex();
		void lock();
		bool try_lock();
		bool unlock();
		HANDLE getHandle() const;
	private:
		HANDLE _handle;
	};
}