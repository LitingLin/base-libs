#pragma once

#include <base/common.h>
#include <string>

namespace Base
{
	ATTRIBUTE_INTERFACE
		std::string getStdCApiErrorString(int errnum);
}
