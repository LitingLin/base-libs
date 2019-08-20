#pragma once
#include <string_view>

namespace Base {
	namespace Logging {
		namespace Details {
			std::string UTF16ToUTF8(std::wstring_view str);
			std::wstring UTF8ToUTF16(std::string_view str);
		}
	}
}
