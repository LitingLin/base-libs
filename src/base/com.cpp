#include <base/com.h>

#include <base/logging.h>
#include <base/registry.h>
#include <base/utils.h>

namespace Base
{
	STDAPI
		RegisterComObject(CLSID clsServer
			, LPCWSTR szDescription
			, LPCWSTR szFileName
			, LPCWSTR szThreadingModel
			, LPCWSTR szServerType)
	{
		// convert CLSID uuid to string and write
		// out subkey as string - CLSID\{}
		//
		wchar_t szCLSID[GUID_STRING_SIZE];
		HRESULT hr = StringFromGUID2(clsServer,
			szCLSID,
			GUID_STRING_SIZE);

		if (FAILED(hr))
			return hr;

		try
		{
			Registry CLSIDRegistry((std::wstring(L"CLSID\\") + szCLSID).c_str(), HKEY_CLASSES_ROOT);
			CLSIDRegistry.setWow64Redirection(true);
			CLSIDRegistry.setString(nullptr, szDescription, uint32_t(wcslen(szDescription)));
			Registry serverRegistry = CLSIDRegistry.createSubKey(szServerType);
			serverRegistry.setString(nullptr, szFileName, uint32_t(wcslen(szFileName)));
			serverRegistry.setString(TEXT("ThreadingModel"), szThreadingModel, uint32_t(wcslen(szThreadingModel)));
		}
		catch (RuntimeException &e)
		{
			return HRESULT_FROM_WIN32(unsigned long(e.getErrorCode()));
		}
		catch (FatalError &e)
		{
			return HRESULT_FROM_WIN32(unsigned long(e.getErrorCode()));
		}
		catch (...)
		{
			return E_FAIL;
		}
		return S_OK;
	}

	STDAPI
		UnregisterComObject(CLSID clsServer)
	{
		// convert CLSID uuid to string and write
		// out subkey as string - CLSID\{}
		//
		OLECHAR szCLSID[GUID_STRING_SIZE];
		HRESULT hr = StringFromGUID2(clsServer,
			szCLSID,
			GUID_STRING_SIZE);

		if (FAILED(hr))
			return hr;

		try
		{
			Registry CLSIDRegistry((std::wstring(L"CLSID\\") + szCLSID).c_str(), HKEY_CLASSES_ROOT);
			CLSIDRegistry.setWow64Redirection(true);
			if (!CLSIDRegistry.remove())
				return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
		catch (RuntimeException &e)
		{
			return HRESULT_FROM_WIN32(unsigned long(e.getErrorCode()));
		}
		catch (FatalError &e)
		{
			return HRESULT_FROM_WIN32(unsigned long(e.getErrorCode()));
		}
		catch (...)
		{
			return E_FAIL;
		}
		return S_OK;
	}

	ObjectsCounter::ObjectsCounter(uint32_t volatile* counter)
		: _counter(counter)
	{
		InterlockedIncrement(_counter);
	}

	ObjectsCounter::~ObjectsCounter()
	{
		InterlockedDecrement(_counter);
	}

	COMInitializer::COMInitializer()
	{
		L_ENSURE_HR(CoInitialize(nullptr));
	}

	COMInitializer::~COMInitializer()
	{
		CoUninitialize();
	}

	// define the prototype of the class factory entry point in a COM dll
	typedef HRESULT(STDAPICALLTYPE* FN_DLLGETCLASSOBJECT)(REFCLSID clsid, REFIID iid, void** ppv);

	HRESULT CreateObjectFromModule(HINSTANCE lib, REFCLSID clsid, IUnknown** ppUnk)
	{
		// the entry point is an exported function
		FN_DLLGETCLASSOBJECT fn = (FN_DLLGETCLASSOBJECT)GetProcAddress(lib, "DllGetClassObject");
		if (fn == NULL)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// create a class factory
		IUnknown * pUnk;
		HRESULT hr = fn(clsid, IID_IUnknown, (void**)(IUnknown**)&pUnk);
		if (SUCCEEDED(hr))
		{
			IClassFactory *pCF = (IClassFactory*)pUnk;
			if (pCF == NULL)
			{
				hr = E_NOINTERFACE;
			}
			else
			{
				// ask the class factory to create the object
				hr = pCF->CreateInstance(NULL, IID_IUnknown, (void**)ppUnk);
				pCF->Release();
			}
		}

		return hr;
	}
}