#pragma once

#include <string>
#include <map>
#include <vector>

namespace Base {
	class File;

	class IniParser
	{
	public:
		class SectionIterator
		{
		public:
			std::string &getSectionName();
			std::string &get(const std::string &name);
			size_t getSize();
		private:
			SectionIterator(std::string *name, std::map<std::string, std::string> *section);
			std::string* _name;
			std::map<std::string, std::string>* _section;
			friend class IniParser;
		};
		IniParser(File* _file);
		SectionIterator getSection(size_t index);
		size_t getNumberOfSection();
	private:
		static int ini_handler(void* user, const char* section,
			const char* name, const char* value,
			int lineno);
		std::vector<std::pair<std::string, std::map<std::string, std::string>>> _sections;
	};

}
