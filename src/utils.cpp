#include <base/utils.h>

#include <base/logging.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Objbase.h>
#pragma comment(lib, "Rpcrt4.lib")
#include <base/logging/win32.h>
#else
#endif


#include <string>
#include <random>

bool operator<(const GUID& left, const GUID& right)
{
	return strncmp((const char*)&left, (const char*)&right, sizeof(GUID)) < 0;
}

namespace Base
{
	bool isMemoryZero(void *buf, size_t size)
	{
		CHECK(size);
		char *buf_ = (char*)buf;
		return buf_[0] == 0 && memcmp(buf_, buf_ + 1, size - 1) == 0;
	}

#ifdef _WIN32
    std::wstring UTF8ToUTF16(const std::string &str)
	{
		if (str.empty())
			return std::wstring();
		int str_size;
		if (str.size() > size_t(std::numeric_limits<int>::max()))
			str_size = std::numeric_limits<int>::max();
		else
			str_size = int(str.size());
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str_size, NULL, NULL);
		std::wstring wideCharString;
		wideCharString.resize(size);
		CHECK_NE_WIN32API(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str_size, &wideCharString[0], size + 1), 0);
		return wideCharString;
	}

	std::string UTF16ToUTF8(const std::wstring &str)
	{
		if (str.empty())
			return std::string();
		int str_size;
		if (str.size() > size_t(std::numeric_limits<int>::max()))
			str_size = std::numeric_limits<int>::max();
		else
			str_size = int(str.size());
		int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str_size, nullptr, 0, NULL, NULL);
		std::string localMultiByteString;
		localMultiByteString.resize(size);
		CHECK_NE_WIN32API(WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str_size, &localMultiByteString[0], size + 1, NULL, NULL), 0);
		return localMultiByteString;
	}
	std::wstring toLowerCase(const std::wstring& str)
	{
		std::wstring lowercaseString = str;

		CharLowerW((wchar_t*)lowercaseString.data());

		return lowercaseString;
	}
	void generateGUID(GUID *guid)
	{
		UuidCreate(guid);
	}
	void generateGUID(wchar_t *str, unsigned str_size)
	{
		GUID guid;
		UuidCreate(&guid);
		GUIDToString(&guid, str, str_size);
	}

    std::wstring generateGUID()
    {
        std::wstring guidString;
        guidString.resize(GUID_STRING_SIZE);
        GUID guid;
        UuidCreate(&guid);
        StringFromGUID2(guid, (wchar_t*)guidString.data(), GUID_STRING_SIZE);

        return guidString;
    }
	void GUIDToString(const GUID *guid, wchar_t *str, unsigned str_size)
	{
		ENSURE_GE(str_size, 39U);
		ENSURE_EQ(StringFromGUID2(*guid, str, 39), 39);
	}
	bool isRunningOn64bitSystem()
	{
#ifdef _M_X64
		return true;
#else
		BOOL isWow64Process;
		if (!IsWow64Process(GetCurrentProcess(), &isWow64Process))
			return false;
		return isWow64Process;
#endif
	}

	bool StringToGUID(const wchar_t *str, unsigned str_size, GUID *guid)
	{
		ENSURE_GE(str_size, 39U);
		const HRESULT hr = IIDFromString(str, guid);
		return SUCCEEDED(hr);
	}
#else
    void generateGUID(GUID *guid)
    {
        int fd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
        ENSURE_NE_STDCAPI(fd, -1);

        ScopeGuard scopeGuard = [&]() {close(fd);};
        int bytesRead = read(fd, guid, 16);
        ENSURE_NE_STDCAPI(bytesRead, -1);
        ENSURE_EQ(bytesRead, 16);
    }
    void generateGUID(char *str, unsigned str_size)
    {
	    GUID guid;
	    generateGUID(&guid);
	    GUIDToString(&guid, str, str_size);
    }
    inline char toHex(uint32_t value)
    {
	    if (value < 10) return '0' + value;
	    else return 'A' + value - 10;
    }
    void uint8ToHexString(uint8_t value, char *str)
    {
	    uint32_t v = value & 0x0FU;
	    str[0] = toHex(v);
	    v = value & 0xF0U;
	    v >>= 4U;
        str[1] = toHex(v);
    }
    void uint64ToHexString(uint64_t value, char *str)
    {
        uint64_t mask = 0x000000000000000FU;
        for (uint32_t i=0;i<16;++i)
        {
            str[i] = toHex((value & mask) >> (4 * i));
            mask <<= 4U;
        }
    }
    void uint32ToHexString(uint32_t value, char *str)
    {
        uint32_t mask = 0x0000000FU;
        for (uint32_t i=0;i<8;++i)
        {
            str[i] = toHex((value & mask) >> (4 * i));
            mask <<= 4U;
        }
    }
    void uint16ToHexString(uint16_t value, char *str)
    {
        uint32_t mask = 0x000FU;
        for (uint32_t i=0;i<4;++i)
        {
            str[i] = toHex((value & mask) >> (4 * i));
            mask <<= 4U;
        }
    }
    void GUIDToString(const GUID *guid, char *str, unsigned str_size)
    {
        ENSURE_GE(str_size, 39U);
        str[0] = '{';
        uint32ToHexString(guid->Data1, str+1);
        str[9] = '-';
        uint16ToHexString(guid->Data2, str+10);
        str[14] = '-';
        uint16ToHexString(guid->Data3, str+15);
        str[19] = '-';
        uint16ToHexString(*(uint16_t*)guid->Data4, str + 20);
        str[24] = '-';
        uint16ToHexString(*(uint16_t*)(guid->Data4 + 2), str + 25);
        uint32ToHexString(*(uint32_t*)(guid->Data4+4),str+29);
        str[37] = '}';
        str[38] = '\0';
    }
    uint32_t parseHex(char str)
    {
        static_assert('a' > 'A' && 'A' > '0', "");
        if (str >= 'a')
            return 10 + str - 'a';
        else if (str >= 'A')
            return 10 + str - 'A';
        else if (str >= '0')
            return str - '0';
    }
    uint32_t hexStringToUint32(const char *str)
    {
	    uint32_t v = 0;
        v |= (parseHex(str[0]) << 28U);
        v |= (parseHex(str[1]) << 24U);
        v |= (parseHex(str[2]) << 20U);
        v |= (parseHex(str[3]) << 16U);
        v |= (parseHex(str[4]) << 12U);
        v |= (parseHex(str[5]) << 8U);
        v |= (parseHex(str[6]) << 4U);
        v |= (parseHex(str[7]));
        return v;
    }
    uint16_t hexStringToUint16(const char *str)
    {
        uint16_t v = 0;
        v |= (parseHex(str[0]) << 12U);
        v |= (parseHex(str[1]) << 8U);
        v |= (parseHex(str[2]) << 4U);
        v |= (parseHex(str[3]));
        return v;
    }
    uint8_t hexStringToUint8(const char *str)
    {
	    uint8_t v = 0;
	    v |= (parseHex(str[0]) << 4U);
	    v |= (parseHex(str[1]));
        return v;
    }

    bool isValidHexCharValue(char hex)
    {
        return (hex >= '0' && hex <= '9') || (hex >= 'A' && hex <= 'F') || (hex >= 'a' && hex <= 'f');
    }

    bool StringToGUID(const char *str, unsigned str_size, GUID *guid)
    {
        ENSURE_GE(str_size, 39U);
	    if (str[0]!='{')
	        return false;
        for (int i=1;i<9;++i)
            if (!isValidHexCharValue(str[i]))
                return false;
        if (str[9]!='-')
            return false;
        for (int i=10;i<14;++i)
            if (!isValidHexCharValue(str[i]))
                return false;
        if (str[14]!='-')
            return false;
        for (int i=15;i<19;++i)
            if (!isValidHexCharValue(str[i]))
                return false;
        if (str[19]!='-')
            return false;
        for (int i=20;i<24;++i)
            if (!isValidHexCharValue(str[i]))
                return false;
        if (str[24]!='-')
            return false;
        for (int i=25;i<37;++i)
            if (!isValidHexCharValue(str[i]))
                return false;
        if (str[37]!='}')
            return false;

        *(uint32_t*)(guid->asArray) = hexStringToUint32(str + 1);
        *(uint16_t*)(guid->asArray + 4) = hexStringToUint16(str + 10);
        *(uint16_t*)(guid->asArray + 6) = hexStringToUint16(str + 15);
        *(uint16_t*)(guid->asArray + 8) = hexStringToUint16(str + 20);
        *(uint16_t*)(guid->asArray + 10) = hexStringToUint16(str + 25);
        *(uint32_t*)(guid->asArray + 12) = hexStringToUint32(str + 29);

        return true;
    }
#endif

	uint8_t generateRandomUint8(uint8_t from, uint8_t to)
	{
		return uint8_t(generateRandomUint16(from, to));
	}

	// !! Warn: Bad Performance
	uint16_t generateRandomUint16(uint16_t from, uint16_t to)
	{
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<uint16_t> dis(from, to);
		return dis(gen);
	}
}