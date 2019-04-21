#include <base/ini_parser.h>

#include <base/file.h>
#include <base/memory_mapped_io.h>
#include <base/config/ini.h>
#include <base/logging.h>


namespace Base
{
	int IniParser::ini_handler(void* user, const char* section,
		const char* name, const char* value,
		int lineno)
	{
		IniParser* this_class = (IniParser*)user;
		this_class->_entries[name] = value;
		
		return 0;
	}
	IniParser::IniParser(File* _file)
	{
		MemoryMappedIO memoryMappedIo(_file);
		auto fileSize = _file->getSize();
		CHECK_EQ(ini_parse_string((const char *)memoryMappedIo.get(), fileSize, ini_handler, this), 0);
	}

	std::string IniParser::get(const std::string& name)
	{
		return _entries[name];
	}
}
