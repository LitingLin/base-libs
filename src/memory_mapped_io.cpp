#define NOMINMAX
#define WIN32_MEAN_AND_LEAN

#include <base/memory_mapped_io.h>
#include <base/logging.h>

#ifdef _WIN32
#include <base/logging/win32.h>
#else
#include <sys/mman.h>
#endif

namespace Base
{
#ifdef _WIN32
	MemoryMappedIO::MemoryMappedIO(File* file, File::DesiredAccess desiredAccess, uint64_t size, uint64_t offset)
		: _file(file)
	{
		DWORD pageProtect;
		switch (desiredAccess)
		{
		case File::DesiredAccess::Read:
			pageProtect = PAGE_READONLY;
			break;
		case File::DesiredAccess::Write:
		case File::DesiredAccess::ReadAndWrite:
			pageProtect = PAGE_READWRITE;
			break;
		default:
			UNREACHABLE_ERROR;
		}
		static_assert(sizeof(ULARGE_INTEGER) == sizeof(size), "");
		ULARGE_INTEGER* pSize = (ULARGE_INTEGER*)&size;
		_hFileMapping = CreateFileMapping(_file->getHANDLE(), NULL, pageProtect, pSize->HighPart, pSize->LowPart, NULL);
		CHECK_WIN32API(_hFileMapping);
		DWORD mapDesiredAccess;
		switch (desiredAccess)
		{
		case File::DesiredAccess::Read:
			mapDesiredAccess = FILE_MAP_READ;
			break;
		case File::DesiredAccess::Write:
			mapDesiredAccess = FILE_MAP_WRITE;
			break;
		case File::DesiredAccess::ReadAndWrite:
			mapDesiredAccess = FILE_MAP_ALL_ACCESS;
			break;
		default:
			UNREACHABLE_ERROR;
		}

		ULARGE_INTEGER* pOffset = (ULARGE_INTEGER*)&offset;
		_ptr = MapViewOfFileEx(_hFileMapping, mapDesiredAccess, pOffset->HighPart, pOffset->LowPart, 0, NULL);
		CHECK_WIN32API_WITH_HANDLER(_ptr, [&] { LOG_IF_FAILED_WIN32API(CloseHandle(_hFileMapping)); });
	}
	
	MemoryMappedIO::~MemoryMappedIO()
	{
		LOG_IF_FAILED_WIN32API(UnmapViewOfFile(_ptr));
		LOG_IF_FAILED_WIN32API(CloseHandle(_hFileMapping));
	}
#else
    MemoryMappedIO::MemoryMappedIO(File* file, File::DesiredAccess desiredAccess, uint64_t size, uint64_t offset)
        : _file(file)
    {
	    CHECK(size > 0 || _file->getSize() >= offset);
	    int prot;
	    switch (desiredAccess)
        {
            case File::DesiredAccess::Read:
                prot = PROT_READ;
                break;
            case File::DesiredAccess::Write:
                prot = PROT_WRITE;
                break;
            case File::DesiredAccess::ReadAndWrite:
                prot = PROT_READ | PROT_WRITE;
                break;
            default:
                UNREACHABLE_ERROR;
        }
        if (size == 0)
            size = _file->getSize() - offset;
        _ptr = mmap(nullptr, size, prot, MAP_SHARED, _file->getFileDiscriptor(), offset);
        CHECK_NE_STDCAPI(_ptr, MAP_FAILED);
        _size = size;
    }

    MemoryMappedIO::~MemoryMappedIO(){
        LOG_IF_EQ_STDCAPI(munmap(_ptr, _size), -1);
    }
#endif
	void* MemoryMappedIO::get()
	{
		return _ptr;
	}
}