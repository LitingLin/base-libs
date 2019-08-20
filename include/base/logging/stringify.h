#pragma once

#include <base/common.h>
#include <string>

namespace Base
{
	namespace Logging
	{
		ATTRIBUTE_INTERFACE
			std::string getStdCApiErrorString(int errnum);
	}
}
