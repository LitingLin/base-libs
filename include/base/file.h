#pragma once

#include <base/porting.h>

#include <stdint.h>
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#endif
#include <vector>
#include <stdio.h>
#include <functional>

namespace Base {
    bool isPathExists(const PLATFORM_STRING_TYPE & path);
#ifdef _WIN32
	std::wstring getWorkDirectory();
	std::wstring getModuleInstanceFullPath(HINSTANCE instance);
	std::wstring getApplicationPath();
	std::wstring getTempPath();
	std::wstring getFullPath(const std::wstring& path);
#else
    bool isFileExists(const std::string& path);
#endif
	std::string getParentPath(const std::string& path);
	std::wstring getParentPath(const std::wstring& path);
	std::string getFileName(const std::string& path);
	std::wstring getFileName(const std::wstring& path);
	bool isURI(const std::string& string);
	bool isURI(const std::wstring&string);
	bool isDirectory(const std::string& string);
	bool isDirectory(const std::wstring&string);
	std::string appendPath(const std::string&path, const std::string&fileName);
	std::wstring appendPath(const std::wstring& path, const std::wstring& fileName);

	std::string getFileExtension(const std::string&path);
	std::wstring getFileExtension(const std::wstring& path);
	std::string getCanonicalPath(const std::string&path);
	std::wstring getCanonicalPath(const std::wstring& path);

	class File
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

		File(const PLATFORM_STRING_TYPE &path, DesiredAccess desiredAccess = DesiredAccess::Read,
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

    class DirectoryIterator
    {
    public:
        DirectoryIterator(const PLATFORM_STRING_TYPE &path);
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

	class SequentialDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
        SequentialDirectoryFileListGetter(const PLATFORM_STRING_TYPE &path);
        bool getFileList(std::vector<PLATFORM_STRING_TYPE> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
        PLATFORM_STRING_TYPE _path;
	};

	class RandomDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
        RandomDirectoryFileListGetter(const PLATFORM_STRING_TYPE &path);
        bool getFileList(std::vector<PLATFORM_STRING_TYPE> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
	    PLATFORM_STRING_TYPE _path;
	};

    bool getDirectoryFileLists(const PLATFORM_STRING_TYPE& path, std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes);
    bool getRandomShuffledDirectoryFileLists(const PLATFORM_STRING_TYPE& path, std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes);
}
