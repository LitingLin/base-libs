#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <base/shared_memory.h>

#include <base/logging.h>
#include <intsafe.h>

namespace Base
{
	SharedMemory::SharedMemory()
		: _hMapFile(nullptr), _ptr(nullptr)
	{
	}

	SharedMemory::SharedMemory(SharedMemory&& other) noexcept
	{
		_hMapFile = other._hMapFile;
		_ptr = other._ptr;
		other._hMapFile = nullptr;
		other._ptr = nullptr;
	}

	SharedMemory::~SharedMemory()
	{
		close();
	}

	void SharedMemory::create(const wchar_t* name, size_t size)
	{
		_hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			_securityAttributes.get(),
			PAGE_READWRITE,          // read/write access
#ifdef _M_X64
			HIDWORD(size),                       // maximum object size (high-order DWORD)
#else
			0,
#endif
			LODWORD(size),                // maximum object size (low-order DWORD)
			name);                 // name of mapping object
		L_CHECK_WIN32API(_hMapFile);

		_ptr = (char*)MapViewOfFile(_hMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			size);
		L_CHECK_WIN32API(_ptr);
	}

	void SharedMemory::open(const wchar_t* name)
	{
		_hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			name);
		L_CHECK_WIN32API(_hMapFile);

		_ptr = (char*)MapViewOfFile(
			_hMapFile,
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,
			0,
			0);

		L_CHECK_WIN32API(_ptr);
	}

	void SharedMemory::close()
	{
		if (_ptr) {
			L_LOG_IF_FAILED_WIN32API(UnmapViewOfFile(_ptr));
			_ptr = nullptr;
		}

		if (_hMapFile) {
			L_LOG_IF_FAILED_WIN32API(CloseHandle(_hMapFile));
			_hMapFile = nullptr;
		}
	}

	char* SharedMemory::get()
	{
		return _ptr;
	}
}