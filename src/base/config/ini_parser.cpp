#include <base/config/ini_parser.h>

#include <base/file.h>
#include <base/memory_mapped_io.h>
#include <base/config/ini.h>
#include <base/logging.h>


namespace Base
{
	IniParser::SectionIterator IniParser::getSection(size_t index)
	{
		auto iterator = _sections.begin() + index;
		return {&iterator->first, &iterator->second};
	}

	size_t IniParser::getNumberOfSection()
	{
		return _sections.size();
	}

	int IniParser::ini_handler(void* user, const char* section,
		const char* name, const char* value,
		int new_line)
	{
		IniParser* this_class = (IniParser*)user;

		if (new_line)
			this_class->_sections.push_back(std::make_pair(section, std::map<std::string, std::string>()));

		auto it = this_class->_sections.rbegin();
		it->second.insert(std::make_pair(name, value));
		
		return 0;
	}

	std::string& IniParser::SectionIterator::getSectionName()
	{
		return *_name;
	}

	std::string& IniParser::SectionIterator::get(const std::string& name)
	{
		return (*_section)[name];
	}

	size_t IniParser::SectionIterator::getSize()
	{
		return (*_section).size();
	}

	IniParser::SectionIterator::SectionIterator(std::string* name, std::map<std::string, std::string>* section)
		: _name(name), _section(section)
	{
	}

	IniParser::IniParser(File* _file)
	{
		MemoryMappedIO memoryMappedIo(_file);
		auto fileSize = _file->getSize();
		L_CHECK_EQ(ini_parse_string((const char *)memoryMappedIo.get(), fileSize, ini_handler, this), 0);
	}
	bool IniParser::SectionIterator::exists(const std::string& name) const
	{
		return _section->find(name) != _section->end();
	}
}
