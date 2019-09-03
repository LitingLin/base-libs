#pragma once

#include <base/common.h>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Rpc.h>
#else

typedef struct _GUID {
	union {
		struct {
			unsigned long Data1;
			unsigned short Data2;
			unsigned short Data3;
			unsigned char Data4[8];
		};
		char asArray[16];
	};
} GUID;

#endif
#include <string>
#include <string_view>
#include <limits>
#include <functional>

ATTRIBUTE_INTERFACE
bool operator<(const GUID& left, const GUID& right);
namespace Base
{
#ifdef _WIN32
	ATTRIBUTE_INTERFACE
	std::wstring UTF8ToUTF16(std::string_view str);
	ATTRIBUTE_INTERFACE
	std::string UTF16ToUTF8(std::wstring_view str);
	//std::string toUpperCase(const std::string &str);
	//std::wstring toUpperCase(const std::wstring &str);
	//std::string toLowerCase(const std::string &str);
	ATTRIBUTE_INTERFACE
	std::wstring toLowerCase(std::wstring_view str);
	ATTRIBUTE_INTERFACE
	void GUIDToString(const GUID* guid, wchar_t* str, unsigned str_size);
	ATTRIBUTE_INTERFACE
	void generateGUID(wchar_t* str, unsigned str_size);
	ATTRIBUTE_INTERFACE
	std::wstring generateGUID();
	ATTRIBUTE_INTERFACE
	bool StringToGUID(const wchar_t* str, unsigned str_size, GUID* guid);
	ATTRIBUTE_INTERFACE
	bool isRunningOn64bitSystem();
#else
	ATTRIBUTE_INTERFACE
	std::string generateGUID();
	ATTRIBUTE_INTERFACE
	void generateGUID(char* str, unsigned str_size);
	ATTRIBUTE_INTERFACE
	void GUIDToString(const GUID * guid, char* str, unsigned str_size);
	ATTRIBUTE_INTERFACE
	bool StringToGUID(const char* str, unsigned str_size, GUID * guid);
#endif
	ATTRIBUTE_INTERFACE
    bool endsWith(std::string_view string, std::string_view ending);
	ATTRIBUTE_INTERFACE
    bool endsWith(std::wstring_view string, std::wstring_view ending);
	ATTRIBUTE_INTERFACE
	bool isMemoryZero(void* buf, size_t size);
	const uint8_t GUID_STRING_SIZE = 39;
	ATTRIBUTE_INTERFACE
	void generateGUID(GUID * guid);
	ATTRIBUTE_INTERFACE
	uint8_t generateRandomUint8(uint8_t from = std::numeric_limits<uint8_t>::min(), uint8_t to = std::numeric_limits<uint8_t>::max());
	ATTRIBUTE_INTERFACE
	uint16_t generateRandomUint16(uint16_t from = std::numeric_limits<uint16_t>::min(), uint16_t to = std::numeric_limits<uint16_t>::max());

	class ScopeGuard {
	public:
		template<class Callable>
		ScopeGuard(Callable&& undo_func) try : f(std::forward<Callable>(undo_func)) {
		}
		catch (...) {
			undo_func();
			throw;
		}
		ScopeGuard(ScopeGuard&& other) : f(std::move(other.f)) {
			other.f = nullptr;
		}
		~ScopeGuard() {
			if (f) f(); // must not throw
		}
		void dismiss() noexcept {
			f = nullptr;
		}
		ScopeGuard(const ScopeGuard&) = delete;
		void operator = (const ScopeGuard&) = delete;
	private:
		std::function<void()> f;
	};
}