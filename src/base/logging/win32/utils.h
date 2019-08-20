#pragma once
#include <string>

namespace Base {
	namespace Logging {
		namespace Details {
			std::string UTF16ToUTF8_Noexcept(const std::wstring& str);
		}
	}
}
