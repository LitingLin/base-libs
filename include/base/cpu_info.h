#pragma once

#include <base/common.h>

namespace Base
{
	ATTRIBUTE_INTERFACE
	bool hasAVX();
	ATTRIBUTE_INTERFACE
	bool hasAVX512f();
}
