#pragma once

#include <base/logging/common.h>
#include <string>

namespace Base
{
	LOGGING_INTERFACE
		std::string getStdCApiErrorString(int errnum);
}
