#pragma once
#include <cstdint>

#include <base/file.h>


namespace Base
{
	class MemoryMappedIO
	{
	public:
        explicit MemoryMappedIO(File *file, File::DesiredAccess desiredAccess = File::DesiredAccess::Read, uint64_t size = 0, uint64_t offset = 0);
		MemoryMappedIO(const MemoryMappedIO&) = delete;
		~MemoryMappedIO();
		void *get();
	private:
	    File *_file;
#ifdef _WIN32
		HANDLE _hFileMapping;
#else
        uint64_t _size;
#endif
		void *_ptr;
	};
}