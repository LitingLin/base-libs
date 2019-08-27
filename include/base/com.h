#pragma once

#define DEFAULT_IUNKNOWN_REF_IMPL \
public: \
ULONG __stdcall AddRef() override { \
	return InterlockedIncrement(&m_cRef); } \
ULONG __stdcall Release() override { \
	ULONG cRef = InterlockedDecrement(&m_cRef); \
	if (cRef == 0) \
		delete this; \
	return cRef; } \
private: \
	long m_cRef = 0;

#define DEFAULT_IUNKNOWN_AGGREGATION_IMPL \
public: \
ULONG __stdcall AddRef() override { \
	if (_owner) \
		return _owner->AddRef(); \
	return InterlockedIncrement(&m_cRef); } \
ULONG __stdcall Release() override { \
	if (_owner) \
		return _owner->Release(); \
	ULONG cRef = InterlockedDecrement(&m_cRef); \
	if (cRef == 0) \
		delete this; \
	return cRef; } \
private: \
	IUnknown *_owner; \
	long m_cRef = 0;

#include <string>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <objbase.h>

namespace Base {
	STDAPI RegisterComObject(CLSID clsServer,
		LPCWSTR szDescription,
		LPCWSTR szFileName,
		LPCWSTR szThreadingModel = L"Both",
		LPCWSTR szServerType = L"InprocServer32");
	STDAPI UnregisterComObject(CLSID clsServer);

	class ObjectsCounter
	{
	public:
		explicit ObjectsCounter(uint32_t volatile *counter);
		virtual ~ObjectsCounter();
	private:
		uint32_t volatile *_counter;
	};

	class COMInitializer
	{
	public:
		COMInitializer();
		~COMInitializer();
	};

	HRESULT CreateObjectFromModule(HINSTANCE lib, REFCLSID clsid, IUnknown** ppUnk);
}
