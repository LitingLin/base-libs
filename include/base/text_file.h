#pragma once

#include <base/file.h>

namespace Base
{
	// TODO: Move to encoding.h
	enum class Encoding
	{
		// Unicode on Windows, Variant on other platforms (depending on the locale of libc, typically UTF-8)
		UTF8,
		UTF16,
		ANSIDefaultCodePage,
	};
	extern const unsigned char UTF16LE_BOM[2];
	extern const unsigned char UTF8LE_BOM[3];

	// TODO:
	class TextFile
	{
	public:
#ifdef _WIN32
		TextFile(const std::wstring& path, File::DesiredAccess desiredAccess = File::DesiredAccess::Read,
		         File::CreationDisposition creationDisposition = File::CreationDisposition::OpenExisting,
		         Encoding encoding = Encoding::UTF8);
#else
		TextFile(const std::string& path, File::DesiredAccess desired_access = File::DesiredAccess::read, File::CreationDisposition creation_disposition = File::CreationDisposition::open_existing, Encoding encoding = Encoding::SystemDefault);
#endif
		void writeUTF8(const char* buffer, uint64_t size);
		void writeUTF16(const wchar_t* buffer, uint64_t size);
		void writeANSIDefaultCodePage(const char* buffer, uint64_t size);
		void readUTF8(char* buffer, uint64_t size);
		void readUTF16(wchar_t* buffer, uint64_t size);
		void readANSIDefaultCodePage(char* buffer, uint64_t size);
	private:
		Encoding _encoding;
		File _file;
	};

}
