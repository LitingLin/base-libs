#pragma once

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
#include <limits>
#include <functional>

bool operator<(const GUID& left, const GUID& right);
namespace Base
{
#ifdef _WIN32
	std::wstring UTF8ToUTF16(const std::string& str);
	std::string UTF16ToUTF8(const std::wstring& str);
	//std::string toUpperCase(const std::string &str);
	//std::wstring toUpperCase(const std::wstring &str);
	//std::string toLowerCase(const std::string &str);
	std::wstring toLowerCase(const std::wstring& str);
	void GUIDToString(const GUID* guid, wchar_t* str, unsigned str_size);
	void generateGUID(wchar_t* str, unsigned str_size);
	std::wstring generateGUID();
	bool StringToGUID(const wchar_t* str, unsigned str_size, GUID* guid);
	bool isRunningOn64bitSystem();
#else
	std::string generateGUID();
	void generateGUID(char* str, unsigned str_size);
	void GUIDToString(const GUID * guid, char* str, unsigned str_size);
	bool StringToGUID(const char* str, unsigned str_size, GUID * guid);
#endif

    bool endsWith(const std::string& string, const std::string& ending);
    bool endsWith(const std::wstring& string, const std::wstring& ending);

	bool isMemoryZero(void* buf, size_t size);
	const uint8_t GUID_STRING_SIZE = 39;
	void generateGUID(GUID * guid);

	uint8_t generateRandomUint8(uint8_t from = std::numeric_limits<uint8_t>::min(), uint8_t to = std::numeric_limits<uint8_t>::max());
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