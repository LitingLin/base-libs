#include "pch.h"

#include <base/registry.h>
#include <base/network.h>
#include <base/utils.h>
#include <base/shared_memory.h>
#include <base/rw_spin_lock.h>
#include <random>
#include <functional>
#include <thread>
#include <base/sync.h>
#include <base/fragmented_udp.h>

#include <uuids.h>
#include <base/process.h>
#include <base/memory_mapped_io.h>
#include <base/file.h>
#include <base/crypto.h>
#include <base/timer.h>

#include <base/data_structures.hpp>

TEST(Utility, wideCharStringToLocalMultiByteString)
{
	std::wstring str = L"test_string";
	EXPECT_EQ(Base::UTF16ToUTF8(str), "test_string");
	str = L"中文";
	EXPECT_EQ(Base::UTF16ToUTF8(str), u8"中文");
	EXPECT_EQ(Base::UTF16ToUTF8(str) + "sd", u8"中文sd");
}

TEST(Utility, localMultiByteStringToWideCharString)
{
	std::string str = "test_string";
	EXPECT_EQ(Base::UTF8ToUTF16(str), L"test_string");
	str = u8"中文";
	EXPECT_EQ(Base::UTF8ToUTF16(str), L"中文");
	EXPECT_EQ(Base::UTF8ToUTF16(str) + L"sd", L"中文sd");
}

TEST(Utility, GUIDToString)
{
	wchar_t groundTruth[] = L"{14DF7173-7746-44B7-9B6E-2C3F9AEF63E8}";
	GUID guid = { 0x14DF7173,0x7746,0x44B7,{ 0x9B,0x6E,0x2C,0x3F,0x9A,0xEF,0x63,0xE8 } };
	wchar_t guidString[39];
	Base::GUIDToString(&guid, guidString, 39);
	EXPECT_EQ(wcsncmp(groundTruth, guidString, 39), 0);
}

TEST(Utility, StringToGUID)
{
	wchar_t guidString[] = L"{14DF7173-7746-44B7-9B6E-2C3F9AEF63E8}";
	GUID groundTruth = { 0x14DF7173,0x7746,0x44B7,{ 0x9B,0x6E,0x2C,0x3F,0x9A,0xEF,0x63,0xE8 } };
	GUID guid;
	EXPECT_TRUE(Base::StringToGUID(guidString, 39, &guid));
	EXPECT_EQ(groundTruth, guid);
}

TEST(Utility, IsRunningOn64bitSystem)
{
	EXPECT_TRUE(Base::isRunningOn64bitSystem());
}

TEST(UDP, UDPServerClientInteroperation)
{
	const uint16_t server_port = 31689;
	const size_t buf_size = 65507;
	const uint32_t timeout = 100;
	char server_buf[buf_size];
	char client_buf[buf_size];
	sockaddr_in fromaddr;
	int fromaddrlen = sizeof(fromaddr);
	Base::UDPServer server(L"localhost", server_port);
	Base::UDPClient client(L"localhost", server_port);
	const char message1[] = "syn";
	int transmission_bytes;
	EXPECT_TRUE(client.send(message1, sizeof(message1), timeout, &transmission_bytes));
	EXPECT_TRUE(server.recvfrom(server_buf, buf_size, &fromaddr, &fromaddrlen, timeout, &transmission_bytes));
	EXPECT_EQ(fromaddrlen, sizeof(fromaddr));
	EXPECT_EQ(strncmp(message1, server_buf, sizeof(message1)), 0);
	const char message2[] = "ack";
	EXPECT_TRUE(server.sendto(message2, sizeof(message2), &fromaddr, fromaddrlen, timeout, &transmission_bytes));
	EXPECT_TRUE(client.recv(client_buf, buf_size, timeout, &transmission_bytes));
	EXPECT_EQ(strncmp(message2, client_buf, sizeof(message2)), 0);
	const size_t message3_len = 6400;
	char* message3 = static_cast<char*>(malloc(message3_len));
	std::default_random_engine generator;
	std::uniform_int_distribution<unsigned short> distribution(0, 255);
	auto rand = std::bind(distribution, generator);
	for (size_t i = 0; i < message3_len; ++i)
		message3[i] = static_cast<char>(rand());
	EXPECT_TRUE(client.send(message3, message3_len, timeout, &transmission_bytes));
	EXPECT_TRUE(server.recvfrom(server_buf, buf_size, &fromaddr, &fromaddrlen, timeout, &transmission_bytes));
	EXPECT_EQ(strncmp(message3, server_buf, message3_len), 0);
}

TEST(UDP, UDPServerClientInteroperationWithEVENT)
{
	const uint16_t server_port = 31689;
	const size_t buf_size = 65507;
	char server_buf[buf_size];
	char client_buf[buf_size];
	sockaddr_in fromaddr;
	int fromaddrlen = sizeof(fromaddr);
	Base::UDPServer server(L"localhost", server_port);
	Base::UDPClient client(L"localhost", server_port);
	const char message1[] = "syn";
	int transmission_bytes;
	HANDLE client_read_event = client.getReadEvent();
	HANDLE server_read_event = server.getReadEvent();
	client.send(message1, sizeof(message1), &transmission_bytes);
	Base::waitForObject(server_read_event);
	EXPECT_TRUE(server.try_recvfrom(server_buf, buf_size, &fromaddr, &fromaddrlen, &transmission_bytes));
	EXPECT_EQ(fromaddrlen, sizeof(fromaddr));
	EXPECT_EQ(strncmp(message1, server_buf, sizeof(message1)), 0);
	const char message2[] = "ack";
	server.sendto(message2, sizeof(message2), &fromaddr, fromaddrlen, &transmission_bytes);
	Base::waitForObject(client_read_event);
	EXPECT_TRUE(client.try_recv(client_buf, buf_size, &transmission_bytes));
	EXPECT_EQ(strncmp(message2, client_buf, sizeof(message2)), 0);
	const size_t message3_len = 6400;
	char* message3 = static_cast<char*>(malloc(message3_len));
	std::default_random_engine generator;
	std::uniform_int_distribution<unsigned short> distribution(0, 255);
	auto rand = std::bind(distribution, generator);
	for (size_t i = 0; i < message3_len; ++i)
		message3[i] = static_cast<char>(rand());
	client.send(message3, message3_len, &transmission_bytes);
	Base::waitForObject(server_read_event);
	server.recvfrom(server_buf, buf_size, &fromaddr, &fromaddrlen, &transmission_bytes);
	EXPECT_EQ(strncmp(message3, server_buf, message3_len), 0);
}

TEST(FragmentedUDP, SendAndReceive)
{
	uint16_t server_port = Base::generateRandomUint16(10000, 40000);
	Base::FragmentedUDPClient client(std::make_unique<Base::UDPClient>(L"127.0.0.1", server_port));
	Base::FragmentedUDPServer server(std::make_unique<Base::UDPServer>(L"127.0.0.1", server_port));
	std::string str;
	uint32_t str_size = 5 * 1024; // 5KB
	str.resize(str_size);
	for (uint32_t i = 0; i < str_size; ++i)
	{
		str[i] = char(i - 6);
	}
	client.send(str);
	sockaddr_in from;
	std::string received_str = server.recvfrom(&from);
	EXPECT_EQ(str, received_str);

	str_size = 7 * 1024; // 7KB
	str.resize(str_size);
	for (uint32_t i = 0; i < str_size; ++i)
	{
		str[i] = char(i - 9);
	}
	server.sendto(str, &from);
	received_str = client.recv();
	EXPECT_EQ(str, received_str);
}

TEST(Registry, WriteGetDelete)
{
	Base::RegistryCurrentSoftware registry(L"test", L"test", L"test");
	registry.setDWORD(L"a", 1);
	uint32_t a1;
	EXPECT_TRUE(registry.getDWORD(L"a", &a1));
	EXPECT_TRUE(registry.remove(L"a"));
	EXPECT_EQ(a1, 1);
	registry.setQWORD(L"a", 1);
	uint64_t a2;
	EXPECT_TRUE(registry.getQWORD(L"a", &a2));
	EXPECT_TRUE(registry.remove(L"a"));
	EXPECT_EQ(a2, 1);
	wchar_t str[] = L"1";
	std::wstring a3;
	registry.setString(L"a", str, 1);
	EXPECT_TRUE(registry.getString(L"a", a3));
	EXPECT_TRUE(registry.remove(L"a"));
	EXPECT_EQ(a3, std::wstring(str));
}

TEST(Registry, WriteAnExistingKey)
{
	Base::RegistryCurrentSoftware registry(L"test", L"test", L"test");
	registry.setDWORD(L"a", 1);
	uint32_t a1;
	EXPECT_TRUE(registry.getDWORD(L"a", &a1));
	EXPECT_EQ(a1, 1);
	registry.setQWORD(L"a", 2);
	uint64_t a2;
	EXPECT_TRUE(registry.getQWORD(L"a", &a2));
	EXPECT_EQ(a2, 2);
	EXPECT_TRUE(registry.remove(L"a"));
}

TEST(Registry, RemoveExistingKeyRecursively)
{
	Base::RegistryCurrentSoftware registry(L"test", L"test", L"test");
	registry.setDWORD(L"a", 1);
	EXPECT_TRUE(registry.remove());
	EXPECT_FALSE(registry.exist());
}

TEST(SharedMemory, Base)
{
	wchar_t uuid_str[Base::GUID_STRING_SIZE * 3];
	Base::generateGUID(uuid_str, Base::GUID_STRING_SIZE);
	uuid_str[Base::GUID_STRING_SIZE - 1] = L'-';
	Base::generateGUID(uuid_str + Base::GUID_STRING_SIZE, Base::GUID_STRING_SIZE);
	uuid_str[Base::GUID_STRING_SIZE * 2 - 1] = L'-';
	Base::generateGUID(uuid_str + 2 * Base::GUID_STRING_SIZE, Base::GUID_STRING_SIZE);
	Base::SharedMemory shared_memory;
	shared_memory.create(uuid_str, 512);
	char* str = shared_memory.get();
	char words[] = "words";
	memcpy(str, words, sizeof(words));
	Base::SharedMemory shared_memory2;
	shared_memory2.open(uuid_str);
	char* str2 = shared_memory2.get();
	EXPECT_EQ(strncmp(str2, words, sizeof(words)), 0);
	shared_memory.close();
	char tests[] = "tests";
	memcpy(str2, tests, sizeof(tests));
	Base::SharedMemory shared_memory3;
	shared_memory3.open(uuid_str);
	shared_memory2.close();
	char* str3 = shared_memory3.get();
	EXPECT_EQ(strncmp(str3, tests, sizeof(tests)), 0);
}

uint64_t g_value = 0;
unsigned g_nthreads = 8;
uint64_t g_nloops = 100000;

unsigned __stdcall spin_lock_rw_thread(void* para)
{
	Base::RWSpinLock* lock = (Base::RWSpinLock*)para;
	for (uint64_t i = 0; i < g_nloops; ++i)
	{
		lock->lock();
		uint64_t c_value = g_value;
		g_value = c_value + 1;
		lock->unlock();
	}
	return 0;
}

TEST(SpinLock, Base)
{
	void* lock = malloc(sizeof(Base::RWSpinLock));
	memset(lock, 0, sizeof(Base::RWSpinLock));
	//new (lock)RWSpinLock;
	std::vector<std::thread> threads;
	threads.reserve(g_nthreads);
	for (unsigned i = 0; i < g_nthreads; ++i)
		threads.push_back(std::thread(spin_lock_rw_thread, lock));
	for (unsigned i = 0; i < g_nthreads; ++i)
		threads[i].join();
	EXPECT_EQ(g_value, g_nthreads * g_nloops);
	free(lock);	
}

TEST(GetParentPath, Case1)
{
	std::wstring case1 = L"//";
	std::wstring result = Base::getParentPath(case1);
	EXPECT_TRUE(result.empty());
}

TEST(GetParentPath, Case2)
{
	std::wstring case2 = L"C://";
	std::wstring result = Base::getParentPath(case2);
	EXPECT_EQ(result, L"C:");
}

TEST(GetParentPath, Case3)
{
	std::wstring case3 = L"C:\\abc.txt";
	std::wstring result = Base::getParentPath(case3);
	EXPECT_EQ(result, L"C:");
}

TEST(GetParentPath, Case4)
{
	std::wstring case4 = L"C:\\abc/abc\\htr";
	std::wstring result = Base::getParentPath(case4);
	EXPECT_EQ(result, L"C:\\abc/abc");
}

TEST(GetParentPath, Case5)
{
	std::wstring test_case = L"C:\\abc/abc\\htr///";
	std::wstring result = Base::getParentPath(test_case);
	EXPECT_EQ(result, L"C:\\abc/abc");
}

TEST(GetParentPath, Case6)
{
	std::wstring test_case = L"C:\\abc/abc\\htr///edb";
	std::wstring result = Base::getParentPath(test_case);
	EXPECT_EQ(result, L"C:\\abc/abc\\htr");
}

TEST(GetFileName, Case1)
{
	std::wstring test_case = L"//segc.exe";
	EXPECT_EQ(Base::getFileName(test_case), L"segc.exe");
}
TEST(GetFileName, Case2)
{
	std::wstring test_case = L"//se\\gc.exe";
	EXPECT_EQ(Base::getFileName(test_case), L"gc.exe");
}
TEST(GetFileName, Case3)
{
	std::wstring test_case = L"//se\\gc.exe\\";
	EXPECT_EQ(Base::getFileName(test_case), L"");
}
TEST(GetFileName, Case4)
{
	std::wstring test_case = L"//se/gc.exe";
	EXPECT_EQ(Base::getFileName(test_case), L"gc.exe");
}
TEST(GetFileName, Case5)
{
	std::wstring test_case = L"\\se/gc.exe";
	EXPECT_EQ(Base::getFileName(test_case), L"gc.exe");
}
TEST(GetFileName, Case6)
{
	std::wstring test_case = L"\\";
	EXPECT_EQ(Base::getFileName(test_case), L"");
}

TEST(GetFullPath, Case1)
{
	std::wstring full_path = Base::getFullPath(L"1.txt");
	std::wstring expected_string = Base::appendPath(Base::getWorkDirectory(), L"1.txt");
	EXPECT_EQ(full_path, expected_string);
}

TEST(GetFullPath, Case2)
{
	std::wstring full_path = Base::getFullPath(L"..\\1.txt");
	std::wstring expected_string = Base::getCanonicalPath(Base::appendPath(Base::appendPath(Base::getWorkDirectory(), L".."), L"1.txt"));
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case3)
{
	std::wstring full_path = Base::getFullPath(L"C:\\1.txt");
	std::wstring expected_string = L"C:\\1.txt";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case4)
{
	std::wstring full_path = Base::getFullPath(L"C:\\df\\..\\");
	std::wstring expected_string = L"C:\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case5)
{
	std::wstring full_path = Base::getFullPath(L"C:\\\\");
	std::wstring expected_string = L"C:\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case6)
{
	std::wstring full_path = Base::getFullPath(L"C:\\\\3.txt\\");
	std::wstring expected_string = L"C:\\3.txt\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case7)
{
	std::wstring full_path = Base::getFullPath(L"D:\\\\3.txt\\..\\");
	std::wstring expected_string = L"D:\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case8)
{
	std::wstring full_path = Base::getFullPath(L"D:\\dfef\\...\\");
	std::wstring expected_string = L"D:\\dfef\\...\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case9)
{
	std::wstring full_path = Base::getFullPath(L"D:\\dfef\\..\\..");
	std::wstring expected_string = L"D:\\";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case10)
{
	std::wstring full_path = Base::getFullPath(L"D:\\dfef\\..\\..\\1.txt");
	std::wstring expected_string = L"D:\\1.txt";
	EXPECT_EQ(full_path, expected_string);
}
TEST(GetFullPath, Case11)
{
	std::wstring full_path = Base::getFullPath(L"D:\\dfef\\..\\..\\\\1.txt");
	std::wstring expected_string = L"D:\\1.txt";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case1)
{
	std::wstring full_path = Base::getCanonicalPath(L"C:\\df\\..\\");
	std::wstring expected_string = L"C:\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case2)
{
	std::wstring full_path = Base::getCanonicalPath(L"C:\\\\");
	std::wstring expected_string = L"C:\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case3)
{
	std::wstring full_path = Base::getCanonicalPath(L"C:\\\\3.txt\\");
	std::wstring expected_string = L"C:\\3.txt\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case4)
{
	std::wstring full_path = Base::getCanonicalPath(L"\\\\3.txt\\");
	std::wstring expected_string = L"\\3.txt\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case5)
{
	std::wstring full_path = Base::getCanonicalPath(L"D:\\\\3.txt\\..\\");
	std::wstring expected_string = L"D:\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case6)
{
	std::wstring full_path = Base::getCanonicalPath(L"D:\\dfef\\...\\");
	std::wstring expected_string = L"D:\\dfef\\...\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case7)
{
	std::wstring full_path = Base::getCanonicalPath(L"D:\\dfef\\..\\..");
	std::wstring expected_string = L"D:\\";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case8)
{
	std::wstring full_path = Base::getCanonicalPath(L"D:\\dfef\\..\\..\\1.txt");
	std::wstring expected_string = L"D:\\1.txt";
	EXPECT_EQ(full_path, expected_string);
}

TEST(getCanonicalPath, Case9)
{
	std::wstring full_path = Base::getCanonicalPath(L"D:\\dfef\\..\\..\\\\1.txt");
	std::wstring expected_string = L"D:\\1.txt";
	EXPECT_EQ(full_path, expected_string);
}

TEST(appendPath, Case1)
{
	std::wstring path = L"C:\\fdf\\";
	std::wstring filename = L"fe.ef";
	EXPECT_EQ(Base::appendPath(path, filename), L"C:\\fdf\\fe.ef");
}

TEST(appendPath, Case2)
{
	std::wstring path = L"C:\\fdf";
	std::wstring filename = L"fe.ef";
	EXPECT_EQ(Base::appendPath(path, filename), L"C:\\fdf\\fe.ef");
}

TEST(appendPath, Case3)
{
	std::wstring path = L"C:\\fdf/";
	std::wstring filename = L"fe.ef";
	EXPECT_EQ(Base::appendPath(path, filename), L"C:\\fdf/fe.ef");
}

TEST(getFileExtension, Case1)
{
	std::wstring filename = L"fe.ef";
	EXPECT_EQ(Base::getFileExtension(filename), L"ef");
}

TEST(getFileExtension, Case2)
{
	std::wstring filename = L"fe.ef/df";
	EXPECT_EQ(Base::getFileExtension(filename), L"");
}

TEST(getFileExtension, Case3)
{
	std::wstring filename = L"fe.ef\\df";
	EXPECT_EQ(Base::getFileExtension(filename), L"");
}

TEST(getFileExtension, Case4)
{
	std::wstring filename = L"fe.ef\\df.rr";
	EXPECT_EQ(Base::getFileExtension(filename), L"rr");
}

#include <random>
TEST(Process, LaunchProcess)
{
	{
		Base::Process process(L"cmd.exe", L"/c echo e > 1.txt");
		Base::File file(L"1.txt");
		Base::MemoryMappedIO memory_mapped_io(&file);
		const char* file_pointer = (const char*)memory_mapped_io.get();
		EXPECT_EQ(*file_pointer, 'e');
	}
	EXPECT_TRUE(DeleteFile(L"1.txt"));
}

TEST(Process, LaunchHighPrivilegeApplication)
{
	// ok
	// Process process(L"taskmgr.exe");
}

TEST(Process, TestProcessMonitor)
{
	Base::ProcessMonitor monitor;
	Base::Process process1(L"powershell.exe", L"-command \"Start-Sleep -Milliseconds 100\"");
	Base::Process process2(L"powershell.exe", L"-command \"Start-Sleep -Milliseconds 300\"");
	monitor.add(process1.getHandle());
	monitor.add(process2.getHandle());

	Base::waitForObject(monitor.getEventHandle());
	HANDLE process1_handle = monitor.pickLastExitProcessHandle();
	EXPECT_EQ(monitor.pickLastExitProcessHandle(), nullptr);
	EXPECT_TRUE(process1_handle);
	EXPECT_EQ(process1_handle, process1.getHandle());
}

#include <spdlog/fmt/fmt.h>

TEST(Process, TestProcessMonitorStressMode)
{
	Base::ProcessMonitor monitor;
	std::vector<Base::Process> processes;

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<uint16_t> dis(20, 500);

	for (uint32_t i = 0; i < 30; ++i)
	{
		processes.emplace_back(L"powershell.exe", std::wstring(fmt::format(L"-command \"Start-Sleep -Milliseconds {}\"", dis(gen))).c_str());
		monitor.add(processes[i].getHandle());
	}

	Sleep(200);
}

std::vector<unsigned char> append(const std::string & left, const std::string & right)
{
	std::vector<unsigned char> res(left.size() + right.size());
	memcpy(res.data(), left.data(), left.size());
	memcpy(res.data() + left.size(), right.data(), right.size());
	return res;
}

std::vector<unsigned char> stringToVector(const std::string & data)
{
	std::vector<unsigned char> res(data.size());
	memcpy(res.data(), data.data(), data.size());
	return res;
}

TEST(Crypto, CreateObject)
{
	Base::Crypto::SHA256 hasher;	
}

TEST(Crypto, Hashdata)
{
	std::string data = "dadsfsdge";
	Base::Crypto::SHA256 hasher;
	std::vector<unsigned char> hash = hasher.hash(stringToVector(data));
	std::string hex = Base::Crypto::BinaryToHexadecimalString(hash);
	EXPECT_EQ(hex, "b1d1524775db1ce3b3d45c5d737b7e5cde28c4eb1f9cbcd7439cfec52abf4486");
}

TEST(Crypto, BinaryToHex)
{
	std::string data_string = "dadsfsdge";
	std::vector<unsigned char> data = stringToVector(data_string);
	std::string hex = Base::Crypto::BinaryToHexadecimalString(data);
	EXPECT_EQ(data, Base::Crypto::HexadecimalStringToBinary(hex));
}

TEST(Crypto, Base64)
{
	std::string data_string = "this is an example";
	std::vector<unsigned char> data = stringToVector(data_string);
	std::string base64 = Base::Crypto::Base64Encode(data);
	EXPECT_EQ(base64, "dGhpcyBpcyBhbiBleGFtcGxl");
	EXPECT_EQ(Base::Crypto::Base64Decode(base64), data);
}

TEST(Crypto, RNG)
{
	Base::Crypto::RNG rng;
	std::vector<unsigned char> bytes = rng.getBytes(5);
}

TEST(Crypto, AES128ECB)
{
	std::string key = "12345678";
	Base::Crypto::SHA256 hasher;
	Base::Crypto::AES128ECB encrypter(hasher.hash(stringToVector(key)));
	std::string data_string = "5951151261232689";
	std::vector<unsigned char> data = stringToVector(data_string);
	std::vector<unsigned char> encryptedData = encrypter.encrypt(data);
	EXPECT_EQ(encrypter.decrypt(encryptedData), data);
}

TEST(Crypto, AES128ECBZeroCopy)
{
	std::string key = "12345678";
	Base::Crypto::SHA256 hasher;
	Base::Crypto::AES128ECB encrypter(hasher.hash(stringToVector(key)));
	std::string data = "5951151261232689";
	unsigned char buf[16];
	unsigned char result[16];
	encrypter.encrypt((const unsigned char*)data.data(), buf, 16);
	encrypter.decrypt(buf, result, 16);
	EXPECT_EQ(memcmp(data.data(), result, 16), 0);
}

TEST(Crypto, DecryptCustomProtocol)
{
	Base::File file(L"..\\bin\\test1.txt");
	Base::MemoryMappedIO mmap(&file);
	uint64_t file_size = *(const uint64_t*)mmap.get();
	EXPECT_EQ(file_size, 8);
	std::string key = "12345678";
	Base::Crypto::SHA256 hasher;
	const char salt[] = "{565f6653-d443-42a7-a9e6-e84a159b1728}";

	Base::Crypto::AES128ECB encrypter(hasher.hash(stringToVector(key + salt)));
	unsigned char buf[16];
	encrypter.decrypt((unsigned char*)mmap.get() + 8, buf, 16);

	EXPECT_EQ(memcmp(buf, "12345678", 8), 0);
}

TEST(Timer, ActiveAgainBeforeTimeout)
{
	Base::Timer timer;
	timer.activate(1000);
	timer.activate(180, 180 * 10000);
	uint64_t tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	uint64_t tock = GetTickCount64();
	EXPECT_GT(tock - tick, 100);
	EXPECT_LT(tock - tick, 500);
}

TEST(Timer, WaitForTimer)
{
	Base::Timer timer;
	timer.activate(0, 100 * 10000);
	uint64_t tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	uint64_t tock = GetTickCount64();
	EXPECT_GT(tock - tick, 50);
	EXPECT_LT(tock - tick, 500);
}

TEST(Timer, InactiveAfterTimeout)
{
	Base::Timer timer;
	timer.activate(0, 100 * 10000);
	Sleep(200);
	timer.inactivate();
}

TEST(Timer, RestartTimer)
{
	Base::Timer timer;
	timer.activate(0, 200 * 10000);
	Sleep(50);
	timer.pause();
	Sleep(500);
	timer.reactivate();
	uint64_t tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	uint64_t tock = GetTickCount64();
	EXPECT_GT(tock - tick, 120);
	EXPECT_LT(tock - tick, 180);
}

TEST(Timer, RestartTimerCase2)
{
	Base::Timer timer;
	timer.activate(100, 200 * 10000);
	Sleep(550);
	timer.pause();
	Sleep(500);
	timer.reactivate();
	uint64_t tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	uint64_t tock = GetTickCount64();
	EXPECT_GT(tock - tick, 30);
	EXPECT_LT(tock - tick, 70);
}
TEST(Timer, RestartTimerCase3)
{
	Base::Timer timer;
	timer.activate(100, 200 * 10000);
	Sleep(150);
	timer.pause();
	Sleep(500);
	timer.reactivate();
	uint64_t tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	uint64_t tock = GetTickCount64();
	EXPECT_GT(tock - tick, 30);
	EXPECT_LT(tock - tick, 70);
	tick = GetTickCount64();
	Base::waitForObject(timer.getHandle());
	tock = GetTickCount64();
	EXPECT_GT(tock - tick, 70);
	EXPECT_LT(tock - tick, 130);
}

TEST(CircularQueue, Case1)
{
	Base::CircularQueue<int> queue(1);
	EXPECT_EQ(queue.size(), 0);
	EXPECT_EQ(queue.capacity(), 1);
	EXPECT_ANY_THROW(queue.pop());
	EXPECT_ANY_THROW(queue.front());
	EXPECT_ANY_THROW(queue.back());

	queue.push(1);
	EXPECT_ANY_THROW(queue.push(2));
	EXPECT_EQ(queue.back(), 1);
	EXPECT_EQ(queue.front(), 1);
	EXPECT_EQ(queue.size(), 1);
	queue.pop();
	EXPECT_ANY_THROW(queue.pop());
	EXPECT_ANY_THROW(queue.front());
	EXPECT_ANY_THROW(queue.back());
	EXPECT_EQ(queue.size(), 0);
	queue.push(2);
	EXPECT_EQ(queue.size(), 1);
	EXPECT_EQ(queue.back(), 2);
	EXPECT_EQ(queue.front(), 2);
	queue.pop();
	queue.push(3);
	queue.pop();
	queue.push(4);
	queue.pop();
	queue.push(5);
	EXPECT_EQ(queue.size(), 1);
	EXPECT_EQ(queue.back(), 5);
	EXPECT_EQ(queue.front(), 5);
}

TEST(CircularQueue, Case2)
{
	Base::CircularQueue<int> queue(3);
	EXPECT_EQ(queue.size(), 0);
	EXPECT_EQ(queue.capacity(), 3);

	queue.push(2);
	queue.push(5);
	EXPECT_EQ(queue.size(), 2);

	queue.push(6);

	EXPECT_EQ(queue.size(), 3);
	EXPECT_EQ(queue.front(), 2);
	EXPECT_EQ(queue.back(), 6);
	EXPECT_EQ(queue[1], 5);
	queue.pop();
	EXPECT_EQ(queue.front(), 5);
	EXPECT_EQ(queue.size(), 2);
	queue.push(33);
	EXPECT_EQ(queue.back(), 33);
	EXPECT_EQ(queue[1], 6);
	EXPECT_EQ(queue[2], 33);
}