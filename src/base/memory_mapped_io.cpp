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
			pageProtect = 0;
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
			mapDesiredAccess = 0;
		}

		ULARGE_INTEGER* pOffset = (ULARGE_INTEGER*)&offset;
		_ptr = MapViewOfFileEx(_hFileMapping, mapDesiredAccess, pOffset->HighPart, pOffset->LowPart, 0, NULL);		
		CHECK_WIN32API_WITH_HANDLER(_ptr, [this] { LOG_IF_FAILED_WIN32API(CloseHandle(_hFileMapping)); });
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
		int flag;
	    switch (desiredAccess)
        {
            case File::DesiredAccess::Read:
                prot = PROT_READ;
				flag = MAP_PRIVATE;
                break;
            case File::DesiredAccess::Write:
                prot = PROT_WRITE;
				flag = MAP_SHARED;
                break;
            case File::DesiredAccess::ReadAndWrite:
                prot = PROT_READ | PROT_WRITE;
				flag = MAP_SHARED;
                break;
            default:
                UNREACHABLE_ERROR;
        }
		if (size == 0) {
			size = _file->getSize() - offset;
			CHECK(size);
		}
        _ptr = mmap(nullptr, size, prot, flag, _file->getFileDescriptor(), offset);
        CHECK_NE_STDCAPI(_ptr, MAP_FAILED) << ". Details: ( size: " << size << ", prot: " << prot << ", fd: " << _file->getFileDescriptor() << ", offset: " << offset << ")";
        _size = size;
    }

    MemoryMappedIO::~MemoryMappedIO(){
        LOG_IF_NOT_NE_STDCAPI(munmap(_ptr, _size), -1);
    }
#endif
	void* MemoryMappedIO::get()
	{
		return _ptr;
	}

	const void* MemoryMappedIO::get() const
	{
		return _ptr;
	}

	BufferedFileOperator::BufferedFileOperator(File* file, File::DesiredAccess desiredAccess, uint64_t position, uint64_t expandingSize)
		: _file(file), _desiredAccess(desiredAccess), _position(position), _expandingSize(expandingSize), _actualFileSize(_file->getSize())
	{
	    if (desiredAccess == File::DesiredAccess::Read)
	    {
	        CHECK(_actualFileSize);
	    }
		else if (desiredAccess == File::DesiredAccess::ReadAndWrite || desiredAccess == File::DesiredAccess::Write)
		{
			if (_actualFileSize == 0)
			{
				if (_expandingSize)
					_file->setSize(_expandingSize);
				else
					_file->setSize(1);
			}
		}

		_memoryMappedIO.reset(new MemoryMappedIO(_file, desiredAccess));
	}

	BufferedFileOperator::~BufferedFileOperator()
	{
		if (_file->getSize() != _actualFileSize) {
			_memoryMappedIO.reset();
			_file->setSize(_actualFileSize);
		}
	}

	void BufferedFileOperator::read(void* buffer, uint64_t size) const
	{
		char* ptr = (char*)(_memoryMappedIO)->get();
		memcpy(buffer, ptr + _position, size);
		_position += size;
	}

	void BufferedFileOperator::write(const void* buffer, uint64_t size)
	{
		auto fileSize = _file->getSize();
		uint64_t newFileSize = 0;
		if (fileSize < _position + size) {
			newFileSize = _position + size;
			if (newFileSize - fileSize < _expandingSize)
				newFileSize = fileSize + _expandingSize;
		}
		if (newFileSize)
		{
			_memoryMappedIO.reset();
			try {
				_file->setSize(newFileSize);
			}
			catch (...)
			{
				Logging::log("Failed to set file size, trying to recover");
			    try {
                    _memoryMappedIO.reset(new MemoryMappedIO(_file, _desiredAccess));
                }
			    catch (...)
                {
					Logging::log("Failed to recover");
                }
				throw;
			}
			_memoryMappedIO.reset(new MemoryMappedIO(_file, _desiredAccess));
		}
		
		char* ptr = (char*)_memoryMappedIO->get();
		memcpy(ptr + _position, buffer, size);
				
		_position += size;

		if (_position > _actualFileSize)
			_actualFileSize = _position;
	}

	void BufferedFileOperator::setPosition(uint64_t position, File::MoveMethod moveMethod) const
	{
		switch (moveMethod)
		{
		case File::MoveMethod::Begin:
			_position = position;
			break;
		case File::MoveMethod::Current:
			_position += position;
			break;
		case File::MoveMethod::End:
			_position = _actualFileSize;
			break;
		default:
			UNREACHABLE_ERROR;
		}
	}

	uint64_t BufferedFileOperator::getPosition() const
	{
		return _position;
	}

	void* BufferedFileOperator::getFilePointer()
	{
		return (char*)_memoryMappedIO->get() + _position;
	}

	const void* BufferedFileOperator::getFilePointer() const
	{
		return (char*)_memoryMappedIO->get() + _position;
	}

	File* BufferedFileOperator::getFile() const
	{
		return _file;
	}
	
	uint64_t BufferedFileOperator::getSize() const
	{
		return _actualFileSize;
	}

	MemoryMappedIO* BufferedFileOperator::getMemoryMappedIO()
	{
		return _memoryMappedIO.get();
	}

	const MemoryMappedIO* BufferedFileOperator::getMemoryMappedIO() const
	{
		return _memoryMappedIO.get();
	}

	void BufferedFileOperator::setSize(uint64_t size)
	{
		_memoryMappedIO.reset();
		try
		{
			_file->setSize(size);
		}
		catch (...)
		{
			_memoryMappedIO.reset(new MemoryMappedIO(_file, _desiredAccess));
			throw;
		}

		_memoryMappedIO.reset(new MemoryMappedIO(_file, _desiredAccess));
		_actualFileSize = size;
		if (_position > _actualFileSize)
			_position = _actualFileSize;
	}
}
