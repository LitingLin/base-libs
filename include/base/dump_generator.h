#pragma once

#include <string>

namespace Base {
	void generateProcessesMiniDumpInPath(const std::wstring &processesPathPrefix, const std::wstring &path, bool excludeSelf);
}