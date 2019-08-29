#include <base/file.h>
#include <random>

#include <base/logging.h>
#ifdef _WIN32
#include <base/logging/win32.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace Base
{
#ifdef WIN32
	bool isPathExists(std::wstring path)
	{
		return GetFileAttributes(path.data()) != INVALID_FILE_ATTRIBUTES;
	}

	std::wstring getModuleInstanceFullPath(HINSTANCE instance)
	{
		std::vector<wchar_t> pathBuf;
		DWORD copied = 0;
		do {
			pathBuf.resize(pathBuf.size() + MAX_PATH);
			copied = GetModuleFileName(instance, &pathBuf.at(0), DWORD(pathBuf.size()));
			CHECK_NE_WIN32API(copied, 0U);
		} while (copied >= pathBuf.size());

		pathBuf.resize(copied);

		std::wstring path(pathBuf.begin(), pathBuf.end());
		return path;
	}

	std::wstring getWorkDirectory()
	{
		DWORD current_directory_size = GetCurrentDirectory(0, nullptr);
		std::wstring work_directory;
		work_directory.resize(current_directory_size - 1);
		ENSURE_WIN32API(GetCurrentDirectory(current_directory_size, &work_directory[0]));
		return work_directory;
	}

	std::wstring getApplicationPath()
	{
		return getModuleInstanceFullPath(nullptr);
	}

	std::wstring getTempPath()
	{
		wchar_t buffer[MAX_PATH + 1];
		DWORD tempPathSize = GetTempPath(MAX_PATH + 1, buffer);
		ENSURE_NE(tempPathSize, DWORD(0));
		return std::wstring(buffer);
	}

	bool isDirectory(std::string_view string)
	{
		return GetFileAttributesA(string.data()) & FILE_ATTRIBUTE_DIRECTORY;
	}

	bool isDirectory(std::wstring_view string)
	{
		return GetFileAttributesW(string.data()) & FILE_ATTRIBUTE_DIRECTORY;
	}

	template <typename CharType> inline
	std::basic_string<CharType> getFullPathHelper(std::basic_string_view<CharType> path)
	{
		if (isURI(path))
			return std::basic_string<CharType>(path);
		std::basic_string<CharType> buffer;
		std::basic_string<CharType> res;
		buffer.resize(MAX_PATH);
		while (true)
		{
			DWORD copied_size = GetFullPathNameW(path.data(), (DWORD)buffer.size() + 1, &buffer[0], nullptr);
			CHECK_NE_WIN32API(copied_size, 0U);
			if (copied_size == buffer.size())
				buffer.resize(buffer.size() * 2);
			else
			{
				res.resize(copied_size);
				memcpy(&res[0], buffer.c_str(), copied_size * sizeof(wchar_t));
				break;
			}
		}
		return res;
	}

	std::wstring getFullPath(std::wstring_view path)
	{
		return getFullPathHelper(path);
	}
#else
    bool isPathExists(std::string_view path)
    {
	    struct stat stat_;
        return stat(path.data(), &stat_) == 0;
    }

    bool isFileExists(std::string_view filePath)
    {
	    struct stat stat_;
	    if (stat(filePath.data(), &stat_) != 0)
            return false;
        return S_ISREG(stat_.st_mode);
    }

#endif

	template <typename CharType>
	struct StaticCharValue
    {
	    const static CharType backSlash;
        const static CharType slash;
        const static CharType colon;
        const static CharType dot;
    };

	template <>
    const char StaticCharValue<char>::backSlash = '\\';
    template <>
    const wchar_t StaticCharValue<wchar_t>::backSlash = L'\\';
    template <>
    const char StaticCharValue<char>::slash = '/';
    template <>
    const wchar_t StaticCharValue<wchar_t>::slash = L'/';
    template <>
    const char StaticCharValue<char>::colon = ':';
    template <>
    const wchar_t StaticCharValue<wchar_t>::colon = L':';
    template <>
    const char StaticCharValue<char>::dot = '.';
    template <>
    const wchar_t StaticCharValue<wchar_t>::dot = L'.';

#ifdef _WIN32
	template <typename CharType> inline
	std::basic_string<CharType> getFileNameHelper(const std::basic_string<CharType>& path)
	{
		size_t backslashpos = path.find_last_of(StaticCharValue<CharType>::backSlash);
		size_t slashpos = path.find_last_of(StaticCharValue<CharType>::slash);
		size_t pos;
		if (backslashpos > slashpos)
		{
			if (backslashpos == path.npos)
				pos = slashpos;
			else
				pos = backslashpos;
		}
		else
		{
			if (slashpos == path.npos)
				pos = backslashpos;
			else
				pos = slashpos;
		}
		if (pos == path.npos)
			pos = 0;
		else
			pos++;
		return path.substr(pos, path.size() - pos);
	}
#else
    template <typename CharType> inline
    std::basic_string<CharType> getFileNameHelper(const std::basic_string<CharType>& path)
    {
        size_t slashpos = path.find_last_of(StaticCharValue<CharType>::slash);
        if (slashpos == path.npos)
            slashpos = 0;
        else
            slashpos++;
        return path.substr(slashpos, path.size() - slashpos);
    }
#endif
	std::string getFileName(const std::string& path)
	{
		return getFileNameHelper(path);
	}

	std::wstring getFileName(const std::wstring& path)
	{
		return getFileNameHelper(path);
	}

	template <typename CharType> inline
	bool isURIHelper(std::basic_string_view<CharType> string)
	{
		size_t slash_pos = string.find_first_of(StaticCharValue<CharType>::colon);
        return slash_pos != string.npos && slash_pos < string.size() - 2 &&
               string[slash_pos + 1] == StaticCharValue<CharType>::slash &&
               string[slash_pos + 2] == StaticCharValue<CharType>::slash;
	}

	bool isURI(std::wstring_view string)
	{
		return isURIHelper(string);
	}

	bool isURI(std::string_view string)
	{
		return isURIHelper(string);
	}

#ifdef _WIN32
	template <typename CharType> inline
	std::basic_string<CharType> appendPathHelper(const std::basic_string<CharType>& path, const std::basic_string<CharType>& fileName)
	{
		if (path[path.size() - 1] == StaticCharValue<CharType>::slash || path[path.size() - 1] == StaticCharValue<CharType>::backSlash)
			return path + fileName;
		else
			return path + StaticCharValue<CharType>::backSlash + fileName;
	}
#else
    template <typename CharType> inline
    std::basic_string<CharType> appendPathHelper(const std::basic_string<CharType>& path, const std::basic_string<CharType>& fileName)
    {
        if (path[path.size() - 1] == StaticCharValue<CharType>::slash)
            return path + fileName;
        else
            return path + StaticCharValue<CharType>::slash + fileName;
    }
#endif

	std::string appendPath(const std::string& path, const std::string& fileName)
	{
		return appendPathHelper(path, fileName);
	}

	std::wstring appendPath(const std::wstring& path, const std::wstring& fileName)
	{
		return appendPathHelper(path, fileName);
	}

#ifdef _WIN32
	template <typename CharType> inline
	std::basic_string<CharType> getFileExtensionHelper(const std::basic_string<CharType>& path)
	{
		size_t dot_pos = path.find_last_of(StaticCharValue<CharType>::dot);
		if (dot_pos == path.npos || path.find(StaticCharValue<CharType>::slash, dot_pos + 1) != path.npos || path.find(StaticCharValue<CharType>::backSlash, dot_pos + 1) != path.npos)
			return std::basic_string<CharType>();
		return path.substr(dot_pos + 1);
	}
#else
    template <typename CharType> inline
    std::basic_string<CharType> getFileExtensionHelper(const std::basic_string<CharType>& path)
    {
        size_t dot_pos = path.find_last_of(StaticCharValue<CharType>::dot);
        if (dot_pos == path.npos || path.find(StaticCharValue<CharType>::slash, dot_pos + 1) != path.npos)
            return std::basic_string<CharType>();
        return path.substr(dot_pos + 1);
    }
#endif

	std::string getFileExtension(const std::string& path)
	{
		return getFileExtensionHelper(path);
	}

	std::wstring getFileExtension(const std::wstring& path)
	{
		return getFileExtensionHelper(path);
	}

#ifdef _WIN32
	template <typename CharType> inline
	std::basic_string<CharType> getCanonicalPathHelper(std::basic_string_view<CharType> path)
	{
        std::basic_string<CharType> buffer;
		buffer.resize(path.size());

		size_t buffer_ind = 0;

		for (size_t i = 0; i < path.size(); ++i)
		{
			if (path[i] == StaticCharValue<CharType>::backSlash)
			{
				if (buffer_ind != 0 && buffer[buffer_ind - 1] == StaticCharValue<CharType>::backSlash)
					continue;
				else {
					buffer[buffer_ind] = StaticCharValue<CharType>::backSlash;
					buffer_ind++;
				}
			}
			else if (path[i] == StaticCharValue<CharType>::slash)
			{
				if (buffer_ind != 0 && buffer[buffer_ind - 1] == StaticCharValue<CharType>::backSlash)
					continue;
				else {
					buffer[buffer_ind] = StaticCharValue<CharType>::backSlash;
					buffer_ind++;
				}
			}
			else if (path[i] == StaticCharValue<CharType>::dot)
			{
				if (i + 1 < path.size() && path[i + 1] == StaticCharValue<CharType>::dot && (i + 2 == path.size() || path[i + 2] == StaticCharValue<CharType>::backSlash || path[i + 2] == StaticCharValue<CharType>::slash) && i >= 1 && (path[i - 1] == StaticCharValue<CharType>::backSlash || path[i - 1] == StaticCharValue<CharType>::slash))
				{
					i++;
					if (buffer_ind < 2)
						continue;
					size_t slash_pos = buffer.find_last_of(StaticCharValue<CharType>::backSlash, buffer_ind - 2);
					if (slash_pos == buffer.npos)
						continue;
					buffer_ind = slash_pos;
				}
				else if (i >= 1 && (path[i - 1] == StaticCharValue<CharType>::backSlash || path[i - 1] == StaticCharValue<CharType>::slash) && (i + 1 == path.size() || path[i + 1] == StaticCharValue<CharType>::backSlash || path[i + 1] == StaticCharValue<CharType>::slash))
				{
					continue;
				}
				else
				{
					buffer[buffer_ind] = path[i];
					buffer_ind++;
				}
			}
			else
			{
				buffer[buffer_ind] = path[i];
				buffer_ind++;
			}
		}
		return buffer.substr(0, buffer_ind);
	}
#else
    template <typename CharType> inline
    std::basic_string<CharType> getCanonicalPathHelper(std::basic_string_view<CharType> path)
    {
        std::basic_string<CharType> buffer;
        buffer.resize(path.size());

        size_t buffer_ind = 0;

        for (size_t i = 0; i < path.size(); ++i)
        {
            if (path[i] == StaticCharValue<CharType>::slash)
            {
                if (buffer_ind != 0 && buffer[buffer_ind - 1] == StaticCharValue<CharType>::slash)
                    continue;
                else {
                    buffer[buffer_ind] = StaticCharValue<CharType>::slash;
                    buffer_ind++;
                }
            }
            else if (path[i] == StaticCharValue<CharType>::dot)
            {
                if (i + 1 < path.size() && path[i + 1] == StaticCharValue<CharType>::dot &&
                (i + 2 == path.size() || path[i + 2] == StaticCharValue<CharType>::slash) && i >= 1 &&
                path[i - 1] == StaticCharValue<CharType>::slash)
                {
                    i++;
                    if (buffer_ind < 2)
                        continue;
                    size_t slash_pos = buffer.find_last_of(StaticCharValue<CharType>::slash, buffer_ind - 2);
                    if (slash_pos == buffer.npos)
                        continue;
                    buffer_ind = slash_pos;
                }
                else if (i >= 1 && path[i - 1] == StaticCharValue<CharType>::slash &&
                (i + 1 == path.size() || path[i + 1] == StaticCharValue<CharType>::slash))
                {
                    continue;
                }
                else
                {
                    buffer[buffer_ind] = path[i];
                    buffer_ind++;
                }
            }
            else
            {
                buffer[buffer_ind] = path[i];
                buffer_ind++;
            }
        }
        return buffer.substr(0, buffer_ind);
    }
#endif

	std::string getCanonicalPath(std::string_view path)
	{
		return getCanonicalPathHelper(path);
	}

	std::wstring getCanonicalPath(std::wstring_view path)
	{
		return getCanonicalPathHelper(path);
	}

#ifdef _WIN32
	DirectoryIterator::DirectoryIterator(PLATFORM_STRING_VIEW_TYPE path)
		: _handle(nullptr), _path(path)
	{
	}

	DirectoryIterator::~DirectoryIterator()
	{
		if (_handle)
			LOG_IF_FAILED_WIN32API(FindClose(_handle));
	}

	bool DirectoryIterator::next(std::wstring& fileName, FileType *fileType, uint64_t *lastFileWriteTime)
	{
		WIN32_FIND_DATA fileData;
		if (!_handle) {
			_handle = FindFirstFileEx((_path + L"\\*").c_str(), FindExInfoBasic, &fileData, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
			if (_handle == INVALID_HANDLE_VALUE) {
				_handle = nullptr;
				return false;
			}
		}
		else
			if (FindNextFile(_handle, &fileData) == FALSE)
				return false;

		fileName = fileData.cFileName;
		if (fileType)
		{
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				*fileType = FileType::Directory;
			else
				*fileType = FileType::File;
		}
		if (lastFileWriteTime)
			*lastFileWriteTime = *((uint64_t*)&fileData.ftLastWriteTime);
		return true;
	}

	void DirectoryIterator::reset()
	{
		if (_handle) {
			LOG_IF_FAILED_WIN32API(FindClose(_handle));
			_handle = nullptr;
		}
	}

#else

    inline uint64_t as_nanoseconds(struct timespec ts) {
        return ts.tv_sec * (uint64_t)1000000000L + ts.tv_nsec;
    }

    DirectoryIterator::DirectoryIterator(std::string_view path) {
        _dir = opendir(path.data());
        CHECK_STDCAPI(_dir);
        _dirfd = dirfd(_dir);
    }
    DirectoryIterator::~DirectoryIterator()
    {
        LOG_IF_NOT_EQ_STDCAPI(closedir(_dir), 0);
    }
    bool DirectoryIterator::next(std::string &fileName, FileType *fileType, uint64_t *lastFileWriteTime)
    {
        while (true) {
            errno = 0;
            struct dirent *dirent = readdir(_dir);
            if (dirent == nullptr) {
                if (errno == 0)
                    return false;
                else
                    THROW_STDCAPI_RUNTIME_EXCEPTION << "readdir() failed";
            }

            int fd = openat(_dirfd, dirent->d_name, O_RDONLY);
            if (fileType || lastFileWriteTime) {
                struct stat stat;
                int rc = fstat(fd, &stat);
                if (rc != 0) {
                    LOGGING_STDCAPI_ERROR << "openat() failed." << "file: " << dirent->d_name;
                    LOG_IF_NOT_EQ_STDCAPI(close(fd), 0);
                    continue;
                }
                if (fileType) {
                    if (S_ISREG(stat.st_mode)) *fileType = FileType::File;
                    else if (S_ISDIR(stat.st_mode)) *fileType = FileType::Directory;
                    else *fileType = FileType::Other;
                }
                if (lastFileWriteTime)
                    *lastFileWriteTime = as_nanoseconds(stat.st_mtim);
            }
            fileName = (char *) (dirent->d_name);
            LOG_IF_NOT_EQ_STDCAPI(close(fd), 0);
            return true;
        }
    }
    void DirectoryIterator::reset()
    {
        rewinddir(_dir);
    }
#endif

	SequentialDirectoryFileListGetter::SequentialDirectoryFileListGetter(std::wstring_view path)
		: _path(path)
	{
	}

	bool SequentialDirectoryFileListGetter::getFileList(std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes)
	{
		return getDirectoryFileLists(_path, fileNames, lastWriteTimes);
	}

	RandomDirectoryFileListGetter::RandomDirectoryFileListGetter(PLATFORM_STRING_VIEW_TYPE path)
		: _path(path)
	{
	}

	bool RandomDirectoryFileListGetter::getFileList(std::vector<PLATFORM_STRING_TYPE>& fileNames,
		std::vector<uint64_t>& lastWriteTimes)
	{
		return getRandomShuffledDirectoryFileLists(_path, fileNames, lastWriteTimes);
	}

	bool getDirectoryFileLists(const PLATFORM_STRING_TYPE& path, std::vector<PLATFORM_STRING_TYPE>& fileNames, std::vector<uint64_t>& lastWriteTimes)
	{
		DirectoryIterator directoryIterator(path);
        PLATFORM_STRING_TYPE tempFileName;
		FileType fileType;
		uint64_t tempLastWriteTime;
		while (directoryIterator.next(tempFileName, &fileType, &tempLastWriteTime))
		{
			if (fileType != FileType::File)
				continue;
			fileNames.push_back(tempFileName);
			lastWriteTimes.push_back(tempLastWriteTime);
		}
		return !fileNames.empty();
	}

	bool getRandomShuffledDirectoryFileLists(const PLATFORM_STRING_TYPE & path, std::vector<PLATFORM_STRING_TYPE> & fileNames, std::vector<uint64_t> & lastWriteTimes)
	{
		std::vector<PLATFORM_STRING_TYPE> sequentialFileNames;
		std::vector<uint64_t> sequentialLastWriteTimes;
		if (getDirectoryFileLists(path, sequentialFileNames, sequentialLastWriteTimes))
		{
			std::vector<size_t> index(sequentialFileNames.size());
			for (size_t i = 0; i < index.size(); ++i)
			{
				index[i] = i;
			}
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(index.begin(), index.end(), g);
			fileNames.resize(sequentialFileNames.size());
			lastWriteTimes.resize(sequentialLastWriteTimes.size());
			for (size_t i = 0; i < index.size(); ++i)
			{
				fileNames[index[i]] = sequentialFileNames[i];
				lastWriteTimes[index[i]] = sequentialLastWriteTimes[i];
			}
			return true;
		}
		return false;
	}

	bool randomPickFile(DirectoryIterator& directoryIterator, PLATFORM_STRING_TYPE& fileName, uint64_t* lastWriteTimePtr,
		std::vector<PLATFORM_STRING_TYPE>* fileNamesPtr, std::vector<uint64_t>* lastWriteTimesPtr, size_t* indexPtr)
	{
		directoryIterator.reset();
		std::vector<PLATFORM_STRING_TYPE> fileNames;
		std::vector<uint64_t> lastWriteTimes;
        PLATFORM_STRING_TYPE tempFileName;
		FileType fileType;
		uint64_t tempLastWriteTime;
		while (directoryIterator.next(tempFileName, &fileType, &tempLastWriteTime))
		{
			if (fileType != FileType::File)
				continue;
			fileNames.push_back(tempFileName);
			lastWriteTimes.push_back(tempLastWriteTime);
		}

		if (fileNames.empty())
			return false;

		// TODO: move out of function
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<size_t> dis(0, fileNames.size() - 1);
		size_t fileIndex = dis(gen);
		fileName = fileNames[fileIndex];

		if (lastWriteTimePtr)
			* lastWriteTimePtr = lastWriteTimes[fileIndex];
		if (fileNamesPtr)
			* fileNamesPtr = fileNames;
		if (lastWriteTimesPtr)
			* lastWriteTimesPtr = lastWriteTimes;
		if (indexPtr)
			* indexPtr = fileIndex;
		return true;
	}

	bool pickNextFile(DirectoryIterator& directoryIterator, PLATFORM_STRING_TYPE& fileName, uint64_t* lastWriteTimePtr)
	{
		FileType fileType;
        PLATFORM_STRING_TYPE tempFileName;
		uint64_t lastWriteTime;
		while (directoryIterator.next(tempFileName, &fileType, &lastWriteTime))
		{
			if (fileType == FileType::File)
				break;
		}
		if (tempFileName.empty())
		{
			directoryIterator.reset();
			while (directoryIterator.next(tempFileName, &fileType, &lastWriteTime))
			{
				if (fileType == FileType::File)
					break;
			}
		}
		if (tempFileName.empty())
			return false;
		else
		{
			fileName = tempFileName;
			if (lastWriteTimePtr)
				* lastWriteTimePtr = lastWriteTime;
			return true;
		}
	}
#ifdef _WIN32
	File::File(std::wstring_view path, DesiredAccess desiredAccess, CreationDisposition creationDisposition)
	{
		int desiredAccess_ = 0;
		switch (desiredAccess)
		{
		case DesiredAccess::Read:
			desiredAccess_ = GENERIC_READ;
			break;
		case DesiredAccess::Write:
			desiredAccess_ = GENERIC_WRITE;
			break;
		case DesiredAccess::ReadAndWrite:
			desiredAccess_ = GENERIC_READ | GENERIC_WRITE;
			break;
		default:
			UNREACHABLE_ERROR;
		}
		int creationDisposition_ = 0;
		switch (creationDisposition)
		{
		case CreationDisposition::CreateAlways:
			creationDisposition_ = CREATE_ALWAYS;
			break;
		case CreationDisposition::CreateNew:
			creationDisposition_ = CREATE_NEW;
			break;
		case CreationDisposition::OpenAlways:
			creationDisposition_ = OPEN_ALWAYS;
			break;
		case CreationDisposition::OpenExisting:
			creationDisposition_ = OPEN_EXISTING;
			break;
		case CreationDisposition::TruncateExisting:
			creationDisposition_ = TRUNCATE_EXISTING;
			break;
		default:
			UNREACHABLE_ERROR;
		}
		_fileHandle = CreateFile(path.data(), desiredAccess_, FILE_SHARE_READ, NULL, creationDisposition_, FILE_ATTRIBUTE_NORMAL, NULL);
		CHECK_NE_WIN32API(_fileHandle, INVALID_HANDLE_VALUE);
	}

	File::File(File&& other) noexcept
		: _fileHandle(other._fileHandle)
	{
		other._fileHandle = nullptr;
	}

	File::~File()
	{
		if (_fileHandle)
			LOG_IF_FAILED_WIN32API(CloseHandle(_fileHandle));
	}

	uint64_t File::getSize() const
	{
		LARGE_INTEGER large_integer;
		CHECK_WIN32API(GetFileSizeEx(_fileHandle, &large_integer));
		return large_integer.QuadPart;
	}

	uint64_t File::read(void* buffer, uint64_t size) const
	{
		char* buffer_ = static_cast<char*>(buffer);
		uint64_t totalReadFileSize = 0;
		while (size)
		{
			DWORD this_read_size;
			if (size > std::numeric_limits<DWORD>::max()) {
				this_read_size = std::numeric_limits<DWORD>::max();
				size -= this_read_size;
			}
			else
			{
				this_read_size = (DWORD)size;
				size = 0;
			}

			DWORD sizeRead;
			CHECK_WIN32API(ReadFile(_fileHandle, buffer_, this_read_size, &sizeRead, nullptr));
			totalReadFileSize += sizeRead;
			buffer_ += this_read_size;
		}
		return totalReadFileSize;
	}

	uint64_t File::write(const void* buffer, uint64_t size)
	{
		const char* buffer_ = static_cast<const char*>(buffer);
		uint64_t totalWriteFileSize = 0;
		while (size)
		{
			DWORD currentWriteSize;
			if (size > std::numeric_limits<DWORD>::max()) {
				currentWriteSize = std::numeric_limits<DWORD>::max();
				size -= currentWriteSize;
			}
			else
			{
				currentWriteSize = (DWORD)size;
				size = 0;
			}

			DWORD sizeWritten;
			CHECK_WIN32API(WriteFile(_fileHandle, buffer_, currentWriteSize, &sizeWritten, nullptr));
			totalWriteFileSize += sizeWritten;
			buffer_ += currentWriteSize;
		}
		return totalWriteFileSize;
	}

	void File::setPosition(uint64_t offset, MoveMethod moveMethod) const
	{
		DWORD dwMoveMethod;
		switch (moveMethod)
		{
		case MoveMethod::Begin:
			dwMoveMethod = FILE_BEGIN;
			break;
		case MoveMethod::Current:
			dwMoveMethod = FILE_CURRENT;
			break;
		case MoveMethod::End:
			dwMoveMethod = FILE_END;
			break;
		default:
			UNREACHABLE_ERROR;
		}
		LARGE_INTEGER large_integer;
		large_integer.QuadPart = offset;
		CHECK_WIN32API(SetFilePointerEx(_fileHandle, large_integer, nullptr, dwMoveMethod));
	}

	uint64_t File::getPosition() const
	{
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER position;
		CHECK_WIN32API(SetFilePointerEx(_fileHandle, zero, &position, FILE_CURRENT));
		return (uint64_t)position.QuadPart;
	}

	uint64_t File::getLastWriteTime() const
	{
		FILETIME lastWriteTime;
		CHECK_WIN32API(GetFileTime(_fileHandle, nullptr, nullptr, &lastWriteTime));
		static_assert(sizeof(uint64_t) == sizeof(FILETIME), "");
		return *((uint64_t*)(&lastWriteTime));
	}

	void File::setSize(uint64_t size)
	{
		setPosition(size);
		CHECK_WIN32API(SetEndOfFile(_fileHandle));
	}

	HANDLE File::getHANDLE()
	{
		return _fileHandle;
	}
#else
    File::File(std::string_view path, DesiredAccess desiredAccess,
               CreationDisposition creationDisposition)
    {
	    int flag = 0;

	    switch (desiredAccess)
        {
            case DesiredAccess::Read:
                flag |= O_RDONLY;
                break;
            case DesiredAccess::Write:
                flag |= O_WRONLY;
                break;
            case DesiredAccess::ReadAndWrite:
                flag |= O_RDWR;
                break;
            default:
                UNREACHABLE_ERROR;
        }

        switch (creationDisposition)
        {
            case CreationDisposition::OpenExisting:
                break;
            case CreationDisposition::CreateAlways:
                flag |= (O_CREAT | O_TRUNC);
                break;
            case CreationDisposition::CreateNew:
                flag |= (O_CREAT | O_EXCL);
                break;
            case CreationDisposition::OpenAlways:
                flag |= O_CREAT;
                break;
            case CreationDisposition::TruncateExisting:
                flag |= O_TRUNC;
                break;
            default:
                UNREACHABLE_ERROR;
        }

		if (flag & O_CREAT || flag & O_TMPFILE)
			_fd = ::open(path.data(), flag, 0666);
		else
			_fd = ::open(path.data(), flag);
	    CHECK_NE_STDCAPI(_fd, -1) << " open() failed with path: " << path << ", flag: " << flag;
    }

    File::~File() {
        if (_fd != -1) {
            LOG_IF_NOT_EQ_STDCAPI(::close(_fd), 0);
        }
    }

    File::File(File &&other) noexcept
    : _fd(other._fd)
    {
        other._fd = -1;
    }

    uint64_t File::getSize() const {
        struct stat64 stat_;
        CHECK_NE_STDCAPI(::fstat64(_fd, &stat_), -1);
        return stat_.st_size;
    }

    uint64_t File::read(void *buffer, uint64_t size) const {
        ssize_t bytesRead = ::read(_fd, buffer, size);
        CHECK_NE_STDCAPI(bytesRead, -1);
        return (uint64_t)bytesRead;
    }

    uint64_t File::write(const void *buffer, uint64_t size) {
        ssize_t bytesWritten = ::write(_fd, buffer, size);
        CHECK_NE_STDCAPI(bytesWritten, -1);
        return (uint64_t)bytesWritten;
    }

    void File::setPosition(uint64_t offset, MoveMethod moveMethod) const {
		int whence;
		switch (moveMethod)
		{
		case MoveMethod::Begin:
			whence = SEEK_SET;
			break;
		case MoveMethod::Current:
			whence = SEEK_CUR;
			break;
		case MoveMethod::End:
			whence = SEEK_END;
			break;
		default:
			UNREACHABLE_ERROR;
		}
        off64_t new_off = ::lseek64(_fd, offset, whence);
        CHECK_NE_STDCAPI(new_off, -1);
        CHECK_EQ_STDCAPI(new_off, offset);
    }

	uint64_t File::getPosition() const
	{
		off64_t off = ::lseek64(_fd, 0, SEEK_CUR);
		CHECK_NE_STDCAPI(off, -1);
		return off;
	}

    uint64_t File::getLastWriteTime() const {
        struct stat64 stat_;
        CHECK_NE_STDCAPI(::fstat64(_fd, &stat_), -1);
        return as_nanoseconds(stat_.st_mtim);
    }

    int File::getFileDescriptor() {
        return _fd;
    }

	void File::setSize(uint64_t size)
	{
		CHECK_NE_STDCAPI(ftruncate64(_fd, size), -1);
	}

#endif

#ifdef _WIN32
	template <typename CharType> inline
	std::basic_string<CharType> getParentPathHelper(const std::basic_string<CharType>& path)
	{
		size_t end_pos = path.size();

		while (true)
		{
			if (!end_pos)
				return std::basic_string<CharType>();
			if (path[end_pos - 1] == StaticCharValue<CharType>::slash || path[end_pos - 1] == StaticCharValue<CharType>::backSlash)
				--end_pos;
			else
				break;
		}

		auto last_slash_pos = path.find_last_of(StaticCharValue<CharType>::slash, end_pos - 1);
		auto last_back_slash_pos = path.find_last_of(StaticCharValue<CharType>::backSlash, end_pos - 1);
		if (last_slash_pos == path.npos)
			last_slash_pos = last_back_slash_pos;
		else if (last_back_slash_pos != path.npos && last_back_slash_pos > last_slash_pos)
			last_slash_pos = last_back_slash_pos;
		if (end_pos > last_slash_pos)
			end_pos = last_slash_pos;

		while (true)
		{
			if (!end_pos)
				return std::basic_string<CharType>();
			if (path[end_pos - 1] == StaticCharValue<CharType>::slash || path[end_pos - 1] == StaticCharValue<CharType>::backSlash)
				--end_pos;
			else
				break;
		}

		return path.substr(0, end_pos);
	}
#else
    template <typename CharType> inline
    std::basic_string<CharType> getParentPathHelper(const std::basic_string<CharType>& path)
    {
        size_t end_pos = path.size();

        while (true)
        {
            if (!end_pos)
                return std::basic_string<CharType>();
            if (path[end_pos - 1] == StaticCharValue<CharType>::slash)
                --end_pos;
            else
                break;
        }

        auto last_slash_pos = path.find_last_of(StaticCharValue<CharType>::slash, end_pos - 1);
        auto last_back_slash_pos = path.find_last_of(StaticCharValue<CharType>::backSlash, end_pos - 1);
        if (last_slash_pos == path.npos)
            last_slash_pos = last_back_slash_pos;
        else if (last_back_slash_pos != path.npos && last_back_slash_pos > last_slash_pos)
            last_slash_pos = last_back_slash_pos;
        if (end_pos > last_slash_pos)
            end_pos = last_slash_pos;

        while (true)
        {
            if (!end_pos)
                return std::basic_string<CharType>();
            if (path[end_pos - 1] == StaticCharValue<CharType>::slash || path[end_pos - 1] == StaticCharValue<CharType>::backSlash)
                --end_pos;
            else
                break;
        }

        return path.substr(0, end_pos);
    }
#endif

	std::string getParentPath(const std::string& path)
	{
		return getParentPathHelper<char>(path);
	}

	std::wstring getParentPath(const std::wstring& path)
	{
		return getParentPathHelper<wchar_t>(path);
	}
}