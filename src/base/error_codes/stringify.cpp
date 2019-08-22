#include <base/error_codes/stringify.h>

#include <cstring>

namespace Base {
#ifdef _WIN32
	std::string getStdCApiErrorString(int errnum)
	{
		char buffer[256];
		std::string errString;
		if (!strerror_s(buffer, errnum))
			return std::string(buffer);
		else
			return std::string();
	}
#else
	std::string getStdCApiErrorString(int errnum)
	{
		return std::string(strerror(errnum));
	}
#endif
}