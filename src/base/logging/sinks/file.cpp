#include <base/logging/sinks/file.h>

#ifdef _WIN32
#include <base/logging/win32/utils.h>
#endif

namespace Base
{
	namespace Logging {
		std::string_view FileSink::getName()
		{
			return "File";
		}

		void FileSink::write(std::string_view message)
		{
			fwrite(message.data(), 1, message.size(), _file);
		}

		void FileSink::flush()
		{
			fflush(_file);
		}

		FileSink::~FileSink()
		{
			fclose(_file);
		}

#ifdef _WIN32
		FileSink::FileSink(std::string_view path)
		{
			std::wstring UTF16Path = Details::UTF8ToUTF16(path);
			_wfopen_s(&_file, UTF16Path.data(), L"wb");
		}

		FileSink::FileSink(std::wstring_view path)
		{
			_wfopen_s(&_file, path.data(), L"wb");
		}
#else
		FileSink::FileSink(std::string_view path)
		{
			_file = fopen(path.data(), 'w');
		}
#endif
	}
}