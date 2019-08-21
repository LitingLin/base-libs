#include <base/logging/macros.h>

#include <sstream>

namespace Base
{
	namespace Logging
	{
		namespace Details
		{
			const char* get_base_file_name(const char* file_name)
			{
				const char* base_file_name = file_name;
				while (*file_name != '\0') {
					if (*file_name == '\\' || *file_name == '/')
						base_file_name = file_name + 1;
					file_name++;
				}

				return base_file_name;
			}
			
			void generateHeaderHelper(std::stringstream& stringstream, const char* file, int line, const char* function)
			{
				stringstream << '[' << get_base_file_name(file) << ':' << line << ' ' << function << "] ";
			}
			
			std::string generateHeader(const char* file, int line, const char* function)
			{
				std::stringstream stringstream;
				generateHeaderHelper(stringstream, file, line, function);
				return stringstream.str();
			}

			std::string generateHeader(const char* file, int line, const char* function, const char* exp)
			{
				std::stringstream stringstream;
				generateHeaderHelper(stringstream, file, line, function);
				stringstream << "Check failed: " << exp;
				return stringstream.str();
			}

			std::string generateHeader(const char* file, int line, const char* function, const char* leftExp, const char* op, const char* rightExp)
			{
				std::stringstream stringstream;
				generateHeaderHelper(stringstream, file, line, function);
				stringstream << "Check failed: " << leftExp << " " << op << " " << rightExp << " ";
				return stringstream.str();
			}
		}
	}
}
