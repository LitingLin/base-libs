#pragma once

#include <string>
#include <map>

namespace Base {
	class File;

	class IniParser
	{
	public:
		IniParser(File* _file);
		std::string get(const std::string& name);
	private:
		static int ini_handler(void* user, const char* section,
			const char* name, const char* value,
			int lineno);
		std::map<std::string, std::string> _entries;
	};

}
