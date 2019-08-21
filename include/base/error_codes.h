#pragma once

namespace Base
{
	enum class ErrorCodeType
	{
		GENERIC, STDCAPI, SQLITE3, CUDA,
#ifdef _WIN32
		WIN32API, HRESULT, NTSTATUS,
#endif
	};
}