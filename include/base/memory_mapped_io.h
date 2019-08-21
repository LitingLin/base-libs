#pragma once
#include <cstdint>
#include <memory>

#include <base/file.h>

namespace Base
{
	// File backend
	class ATTRIBUTE_INTERFACE MemoryMappedIO
	{
	public:
        explicit MemoryMappedIO(File *file, File::DesiredAccess desiredAccess = File::DesiredAccess::Read, uint64_t size = 0, uint64_t offset = 0);
		MemoryMappedIO(const MemoryMappedIO&) = delete;
		~MemoryMappedIO();
		void *get();
		const void* get() const;
	private:
	    File *_file;
#ifdef _WIN32
		HANDLE _hFileMapping;
#else
        uint64_t _size;
#endif
		void *_ptr;
	};

	class ATTRIBUTE_INTERFACE BufferedFileOperator
	{
	public:
		constexpr static uint64_t ExpandingSize = 4ULL * 1024ULL * 1024ULL; // 4MB

		explicit BufferedFileOperator(File* file, File::DesiredAccess desiredAccess = File::DesiredAccess::Read, uint64_t position = 0, uint64_t expandingSize = ExpandingSize);
		~BufferedFileOperator();
		void read(void* buffer, uint64_t size) const;
		void write(const void* buffer, uint64_t size);
		void setPosition(uint64_t position, File::MoveMethod moveMethod = File::MoveMethod::Begin) const;
		uint64_t getPosition() const;
		void* getFilePointer();
		const void* getFilePointer() const;
		File* getFile() const;
		uint64_t getSize() const;
		MemoryMappedIO* getMemoryMappedIO();
		const MemoryMappedIO* getMemoryMappedIO() const;
		void setSize(uint64_t size);
	private:
		File* _file;
		std::unique_ptr<MemoryMappedIO> _memoryMappedIO;
		File::DesiredAccess _desiredAccess;
		mutable uint64_t _position;
		uint64_t _expandingSize;

		uint64_t _actualFileSize;
	};
}