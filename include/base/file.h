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
namespace Base {
#ifdef _WIN32
	bool isPathExists(const std::wstring& path);
	std::wstring getWorkDirectory();
	std::wstring getModuleInstanceFullPath(HINSTANCE instance);
	std::wstring getApplicationPath();
	std::wstring getTempPath();
#else

#endif

	std::string getParentPath(const std::string& path);
	std::wstring getParentPath(const std::wstring& path);
	std::string getFileName(const std::string& path);
	std::wstring getFileName(const std::wstring& path);
	bool isURI(const std::string& string);
	bool isURI(const std::wstring&string);
	bool isDirectory(const std::string& string);
	bool isDirectory(const std::wstring&string);
	std::string getFullPath(const std::string& path);
	std::wstring getFullPath(const std::wstring&path);
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

	extern unsigned char UTF16LE_BOM[2];

	class File
	{
	public:
		//                          |                    When the file...
		// This argument:           |             Exists            Does not exist
		// -------------------------+------------------------------------------------------
		// CREATE_ALWAYS            |            Truncates             Creates
		// CREATE_NEW         +-----------+        Fails               Creates
		// OPEN_ALWAYS     ===| does this |===>    Opens               Creates
		// OPEN_EXISTING      +-----------+        Opens                Fails
		// TRUNCATE_EXISTING        |            Truncates              Fails
		enum class Mode : uint32_t
		{
			read = 0x00010000UL,
			write = 0x00100000UL,
			both = read | write,
			create_always = 0x00000001UL,
			create_new = 0x00000010UL,
			open_existing = 0, // default
			open_always = 0x00000100UL,
			truncate_existing = 0x00001000UL
		};
#ifdef _WIN32
		File(const std::wstring &path, Mode mode = Mode::read);
#else
		File(const std::string& path, Mode mode = Mode::read);
#endif
		File(const File &path) = delete;
		File(File &&path) noexcept;
		~File();
		uint64_t getSize() const;
		uint64_t read(unsigned char *buffer, uint64_t offset, uint64_t size) const;
		uint64_t write(const unsigned char *buffer, uint64_t offset, uint64_t size);
		uint64_t getLastWriteTime() const;
		HANDLE getHANDLE();
	private:
		HANDLE _fileHandle;
	};

	File::Mode operator~(File::Mode value);
	File::Mode operator&(File::Mode left, File::Mode right);
	File::Mode operator|(File::Mode left, File::Mode right);
}