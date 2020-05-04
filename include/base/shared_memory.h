#pragma once

#include <base/common.h>
typedef void * HANDLE;

namespace Base
{
	class ATTRIBUTE_INTERFACE SharedMemory
	{
	public:
		SharedMemory();
		SharedMemory(const SharedMemory &) = delete;
		SharedMemory(SharedMemory &&) noexcept;
		~SharedMemory();
		bool createOrOpen(const wchar_t *name, size_t size);
		void open(const wchar_t *name);
		void close();
		size_t getSize();
		unsigned char* get();
	private:
		HANDLE _hMapFile;
		void *_ptr;
	};
}