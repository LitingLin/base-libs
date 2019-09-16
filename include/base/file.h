#pragma once

#include <base/common.h>
#include <base/porting.h>

#include <stdint.h>
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dirent.h>
#endif
#include <vector>
#include <stdio.h>
#include <functional>

namespace Base {
	ATTRIBUTE_INTERFACE
    bool isPathExists(PLATFORM_STRING_VIEW_TYPE path);
#ifdef _WIN32
	ATTRIBUTE_INTERFACE
	std::wstring getWorkDirectory();
	ATTRIBUTE_INTERFACE
	std::wstring getModuleInstanceFullPath(HINSTANCE instance);
	ATTRIBUTE_INTERFACE
	std::wstring getApplicationPath();
	ATTRIBUTE_INTERFACE
	std::wstring getTempPath();
	ATTRIBUTE_INTERFACE
	std::wstring getFullPath(std::wstring_view path);
#else
	ATTRIBUTE_INTERFACE
    bool isFileExists(const std::string& path);
#endif
	ATTRIBUTE_INTERFACE
	std::string getParentPath(const std::string& path);
	ATTRIBUTE_INTERFACE
	std::wstring getParentPath(const std::wstring& path);
	ATTRIBUTE_INTERFACE
	std::string getFileName(const std::string& path);
	ATTRIBUTE_INTERFACE
	std::wstring getFileName(const std::wstring& path);
	ATTRIBUTE_INTERFACE
	bool isURI(std::string_view string);
	ATTRIBUTE_INTERFACE
	bool isURI(std::wstring_view string);
	ATTRIBUTE_INTERFACE
	bool isDirectory(std::string_view string);
	ATTRIBUTE_INTERFACE
	bool isDirectory(std::wstring_view string);
	ATTRIBUTE_INTERFACE
	std::string appendPath(const std::string& path, const std::string& fileName);
	ATTRIBUTE_INTERFACE
	std::wstring appendPath(const std::wstring& path, const std::wstring& fileName);

	ATTRIBUTE_INTERFACE
	std::string getFileExtension(const std::string& path);
	ATTRIBUTE_INTERFACE
	std::wstring getFileExtension(const std::wstring& path);
	ATTRIBUTE_INTERFACE
	std::string getCanonicalPath(std::string_view path);
	ATTRIBUTE_INTERFACE
	std::wstring getCanonicalPath(std::wstring_view path);
		
	class ATTRIBUTE_INTERFACE File
	{
	public:
		enum class DesiredAccess
		{
			Read,
			Write,
			ReadAndWrite
		};

		//                          |                    When the file...
		// This argument:           |             Exists            Does not exist
		// -------------------------+------------------------------------------------------
		// CREATE_ALWAYS            |            Truncates             Creates
		// CREATE_NEW         +-----------+        Fails               Creates
		// OPEN_ALWAYS     ===| does this |===>    Opens               Creates
		// OPEN_EXISTING      +-----------+        Opens                Fails
		// TRUNCATE_EXISTING        |            Truncates              Fails
		enum class CreationDisposition
		{
			CreateAlways,
			CreateNew,
			OpenAlways,
			OpenExisting,
			TruncateExisting
		};

		enum class MoveMethod
		{
			Begin,
			Current,
			End
		};

		File(PLATFORM_STRING_VIEW_TYPE path, DesiredAccess desiredAccess = DesiredAccess::Read,
				CreationDisposition creationDisposition = CreationDisposition::OpenExisting);
		File(const File &) = delete;
		File(File && other) noexcept;
		~File();
		uint64_t getSize() const;
		uint64_t read(void* buffer, uint64_t size) const;
		uint64_t write(const void* buffer, uint64_t size);
		void setPosition(uint64_t offset, MoveMethod moveMethod = MoveMethod::Begin) const;
		uint64_t getPosition() const;
		uint64_t getLastWriteTime() const;
		void setSize(uint64_t size);
#ifdef _WIN32
		HANDLE getHANDLE();
#else
		int getFileDescriptor();
#endif
	private:
#ifdef _WIN32
		HANDLE _fileHandle;
#else
		int _fd;
#endif
	};

    enum class FileType
    {
        Directory,
        File,
        Other
    };

    class ATTRIBUTE_INTERFACE DirectoryIterator
    {
    public:
        DirectoryIterator(PLATFORM_STRING_VIEW_TYPE path);
        ~DirectoryIterator();
        bool next(PLATFORM_STRING_TYPE &fileName, FileType *fileType, uint64_t *lastFileWriteTime);
        void reset();
    private:
#ifdef _WIN32
        HANDLE _handle;
        std::wstring _path;
#else
        DIR* _dir;
        int _dirfd;
#endif
    };

    class IDirectoryFileListGetter
	{
	public:
		virtual ~IDirectoryFileListGetter() = default;
		virtual bool getFileList(std::vector<PLATFORM_STRING_TYPE> &fileNames, std::vector<uint64_t> &lastWriteTimes) = 0;
	};
		
	class ATTRIBUTE_INTERFACE SequentialDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
        SequentialDirectoryFileListGetter(PLATFORM_STRING_VIEW_TYPE path);
        bool getFileList(std::vector<PLATFORM_STRING_TYPE> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
        PLATFORM_STRING_TYPE _path;
	};
		
	class ATTRIBUTE_INTERFACE RandomDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
        RandomDirectoryFileListGetter(PLATFORM_STRING_VIEW_TYPE path);
        bool getFileList(std::vector<PLATFORM_STRING_TYPE> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
	    PLATFORM_STRING_TYPE _path;
	};
	
	ATTRIBUTE_INTERFACE
    bool getDirectoryFileLists(const PLATFORM_STRING_TYPE& path, std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes);
	ATTRIBUTE_INTERFACE
    bool getRandomShuffledDirectoryFileLists(const PLATFORM_STRING_TYPE& path, std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes);
}
