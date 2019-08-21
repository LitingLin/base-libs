#pragma once

#include <base/event.h>
#include <mutex>
#include <vector>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <concurrent_queue.h>
#include <map>

namespace Base
{
	class ATTRIBUTE_INTERFACE OpenProcessGuard
	{
	public:
		OpenProcessGuard(DWORD dwDesiredAccess, DWORD dwProcessId);
		~OpenProcessGuard();
		HANDLE getHANDLE() const;
	private:
		HANDLE handle;
	};
		
	class ATTRIBUTE_INTERFACE Process
	{
	public:
		Process(const wchar_t *application, const wchar_t *command = nullptr);
		Process(const Process &) = delete;
		Process(Process &&other) noexcept;
		~Process();
		HANDLE getHandle() const;
		unsigned long getId() const;
		bool isActive() const;
		unsigned long getExitCode() const;
	private:
		HANDLE _processHandle;
	};

	class ATTRIBUTE_INTERFACE ProcessMonitor
	{
	public:
		ProcessMonitor();
		ProcessMonitor(const ProcessMonitor&) = delete;
		ProcessMonitor(ProcessMonitor&&) = delete;
		~ProcessMonitor();
		void add(HANDLE process_handle);
		HANDLE getEventHandle() const;
		HANDLE pickLastExitProcessHandle();
	private:
		Concurrency::concurrent_queue<HANDLE> _exitedProcessHandle;
		Event _event;
		HANDLE _finalize_signal;
		std::mutex _mutex;
		std::map<DWORD,HANDLE> _running_threads;
		unsigned static __stdcall monitor_worker(void *);
	};

	ATTRIBUTE_INTERFACE
	std::vector<uint32_t> getSystemProcessesIds();
	ATTRIBUTE_INTERFACE
	std::vector<HMODULE> getProcessModules(HANDLE hProcess);
	ATTRIBUTE_INTERFACE
	std::wstring getModuleFullPath(HANDLE hProcess, HMODULE hModule);
	ATTRIBUTE_INTERFACE
	std::wstring getProcessFullPath(HANDLE hProcess);
	ATTRIBUTE_INTERFACE
	std::wstring getProcessName(HANDLE hProcess);
	ATTRIBUTE_INTERFACE
	std::wstring enumerateAllProcessInfomation();
}
