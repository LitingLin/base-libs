#pragma once

#include <base/common.h>
#include <string>

namespace Base {
	ATTRIBUTE_INTERFACE
	void generateProcessesMiniDumpInPath(const std::wstring &processesPathPrefix, const std::wstring &path, bool excludeSelf);
}