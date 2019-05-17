#pragma once

#include <string>
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
#ifdef _WIN32
	bool isPathExists(const std::wstring& path);
	std::wstring getWorkDirectory();
	std::wstring getModuleInstanceFullPath(HINSTANCE instance);
	std::wstring getApplicationPath();
	std::wstring getTempPath();
	std::wstring getFullPath(const std::wstring& path);
#else
    bool isPathExists(const std::string& path);
    bool isFileExists(const std::string& Path);
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

#ifdef _WIN32
	class DirectoryIterator
	{
	public:
		DirectoryIterator(const std::wstring &path);
		~DirectoryIterator();
		bool next(std::wstring &fileName, bool &isDirectory, uint64_t &lastFileWriteTime);
		void reset();
	private:
		HANDLE _handle;
		std::wstring _path;
	};

	class IDirectoryFileListGetter
	{
	public:
		virtual ~IDirectoryFileListGetter() = default;
		virtual bool getFileList(std::vector<std::wstring> &fileNames, std::vector<uint64_t> &lastWriteTimes) = 0;
	};

	class SequentialDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
		SequentialDirectoryFileListGetter(const std::wstring &path);
		bool getFileList(std::vector<std::wstring> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
		std::wstring _path;
	};

	class RandomDirectoryFileListGetter : public IDirectoryFileListGetter
	{
	public:
		RandomDirectoryFileListGetter(const std::wstring &path);
		bool getFileList(std::vector<std::wstring> &fileNames, std::vector<uint64_t> &lastWriteTimes) override;
	private:
		std::wstring _path;
	};

	bool getDirectoryFileLists(const std::wstring& path, std::vector<std::wstring>& fileNames, std::vector<uint64_t>& lastWriteTimes);
	bool getRandomShuffledDirectoryFileLists(const std::wstring& path, std::vector<std::wstring>& fileNames, std::vector<uint64_t>& lastWriteTimes);
#endif

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

#ifdef _WIN32
		File(const std::wstring &path, DesiredAccess desiredAccess = DesiredAccess::Read, 
				CreationDisposition creationDisposition = CreationDisposition::OpenExisting);
#else
		File(const std::string& path, DesiredAccess desiredAccess = DesiredAccess::Read,
		        CreationDisposition creationDisposition = CreationDisposition::OpenExisting);
#endif
		File(const File &) = delete;
		File(File && other) noexcept;
		~File();
		uint64_t getSize() const;
		uint64_t read(void* buffer, uint64_t size) const;
		uint64_t write(const void* buffer, uint64_t size);
		void seek(uint64_t offset);
		uint64_t getLastWriteTime() const;
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
}
