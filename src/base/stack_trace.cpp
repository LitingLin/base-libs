#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#endif
#include <string>
#include <limits>
#include <mutex>
#include <memory>
#include <functional>

namespace Base
{
	std::mutex stack_trace_locker;
	enum class SymbolInfoState{ Uninitialized, Initialized, Failed};
#ifdef _WIN32
    HANDLE hProcess = GetCurrentProcess();
	std::string getStackTrace()
	{
		std::string message;
		std::unique_ptr<SYMBOL_INFO, std::function<void(SYMBOL_INFO*)>> symbol_info((SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + (MAX_SYM_NAME - 1) * sizeof(char)), [](SYMBOL_INFO * symbol_info) { if (symbol_info) free(symbol_info); });
		if (!symbol_info)
			return "malloc() failed during capturing call stack.";
		constexpr unsigned short max_frames = std::numeric_limits<unsigned short>::max();
		std::unique_ptr<void*, std::function<void(void**)>> stacks((void**)malloc(sizeof(void*) * max_frames), [](void** stacks) { if(stacks) free(stacks); });
		if (!stacks)
			return "malloc() failed during capturing call stack.";

		{
			std::lock_guard<std::mutex> stack_trace_lock_guard(stack_trace_locker);

			static SymbolInfoState symbolInfoState = SymbolInfoState::Uninitialized;
			if (symbolInfoState == SymbolInfoState::Uninitialized)
			{
				if (!SymInitialize(hProcess, nullptr, TRUE))
				{
					message += ("Warning: SymInitialize() failed with "
						"Win32 error code: " + std::to_string(GetLastError()) + "\n");
					symbolInfoState = SymbolInfoState::Failed;
				}
				else
					symbolInfoState = SymbolInfoState::Initialized;
			}
			else if (symbolInfoState == SymbolInfoState::Initialized)
			{
				if (!SymRefreshModuleList(hProcess))
				{
					message += ("Warning: SymRefreshModuleList() failed with "
						"Win32 error code: " + std::to_string(GetLastError()) + "\n");
				}
			}
			else if (symbolInfoState == SymbolInfoState::Failed)
			{
				message += ("Info: Failed to initialize the symbol handler.");
			}

			unsigned short frames = CaptureStackBackTrace(1, max_frames, stacks.get(), nullptr);
			const unsigned address_buffer_length = sizeof(ptrdiff_t) * 2 + 1;
			char buffer[address_buffer_length];

			for (unsigned short i = 0; i < frames; ++i)
			{
				memset(symbol_info.get(), 0, sizeof(SYMBOL_INFO));
				symbol_info->MaxNameLen = MAX_SYM_NAME;
				symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);

				if (!SymFromAddr(hProcess, (DWORD64)stacks.get()[i], 0, symbol_info.get()))
				{
					message += "Warning: SymFromAddr() failed with "
						"Win32 error code: " + std::to_string(GetLastError()) + "\n";
				}
				snprintf(buffer, address_buffer_length, "%llx", symbol_info->Address);
				message += std::to_string(i) + "\t0x" + buffer + "\t" + (symbol_info->Name[0] == '\0' ? "(unknown)" : symbol_info->Name) + "\n";
			}
		}

		return message;
	}
#else
    std::string getStackTrace()
    {
	    int max_frames = std::numeric_limits<uint16_t>::max();
	    std::unique_ptr<void*, std::function<void(void**)>> stacks((void**)malloc(sizeof(void*) * max_frames), [](void** stacks) { if(stacks) free(stacks); });
	    if (!stacks)
            return "malloc() failed during capturing call stack.";

        int frames = backtrace(stacks.get(), max_frames);
        std::unique_ptr<char *, std::function<void(char **)>> symbols(backtrace_symbols(stacks.get(), frames), [](char **symbols) { if (symbols) free(symbols);});
        if (!symbols)
            return "malloc() failed during capturing call stack.";

        std::string message;

        const unsigned address_buffer_length = sizeof(ptrdiff_t) * 2 + 1;
        char buffer[address_buffer_length];

        for (int i = 1;i<frames;++i)
        {
            snprintf(buffer, address_buffer_length, "%llx", (long long unsigned int)stacks.get()[i]);
            message += std::to_string(i - 1) + "\t0x" + buffer + '\t' + symbols.get()[i] + '\n';
        }
        return message;
    }
#endif
}