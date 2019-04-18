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

template <typename F>
struct ScopeExit {
    explicit ScopeExit(F f) : f(f) {}
    ~ScopeExit() { f(); }
    F f;
};

template <typename F>
ScopeExit<F> MakeScopeExit(F f) {
    return ScopeExit<F>(f);
};

#define STRING_JOIN2(arg1, arg2) DO_STRING_JOIN2(arg1, arg2)
#define DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2
#define ON_SCOPE_EXIT(code) \
    auto STRING_JOIN2(scope_exit_, __LINE__) = MakeScopeExit([=](){code;})

bool operator<(const GUID& left, const GUID& right);
namespace Base
{
#ifdef _WIN32
	std::wstring UTF8ToUTF16(const std::string &str);
	std::string UTF16ToUTF8(const std::wstring &str);
	//std::string toUpperCase(const std::string &str);
	//std::wstring toUpperCase(const std::wstring &str);
	//std::string toLowerCase(const std::string &str);
	std::wstring toLowerCase(const std::wstring &str);
	void GUIDToString(const GUID *guid, wchar_t *str, unsigned str_size);
	void generateGUID(wchar_t *str, unsigned str_size);
	std::wstring generateGUID();
	bool StringToGUID(const wchar_t *str, unsigned str_size, GUID *guid);
	bool isRunningOn64bitSystem();
#else
	std::string generateGUID();
	void generateGUID(char* str, unsigned str_size);
	void GUIDToString(const GUID* guid, char* str, unsigned str_size);
	bool StringToGUID(const char* str, unsigned str_size, GUID* guid);
#endif

	bool isMemoryZero(void *buf, size_t size);
	const uint8_t GUID_STRING_SIZE = 39;
	void generateGUID(GUID *guid);

	uint8_t generateRandomUint8(uint8_t from = std::numeric_limits<uint8_t>::min(), uint8_t to = std::numeric_limits<uint8_t>::max());
	uint16_t generateRandomUint16(uint16_t from = std::numeric_limits<uint16_t>::min(), uint16_t to = std::numeric_limits<uint16_t>::max());
}