#include <base/text_file.h>

namespace Base
{
	const unsigned char UTF16LE_BOM[2] = { 0xFF,0xFE };
	const unsigned char UTF8_COM[3] = { 0xEF, 0xBB, 0xBF };

	TextFile::TextFile(const std::wstring& path, File::DesiredAccess desiredAccess,
		File::CreationDisposition creationDisposition, Encoding encoding)
		: _encoding(encoding), _file(path, desiredAccess, creationDisposition)
	{
	}

	void TextFile::writeUTF8(const char* buffer, uint64_t size)
	{
	}
}
