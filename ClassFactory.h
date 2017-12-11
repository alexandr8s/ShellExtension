#pragma once

#include <Windows.h>

class ClassFactory: public IClassFactory
{
protected:
	DWORD m_objRefCount;
	~ClassFactory();
public:
	ClassFactory();

	//IUnknown methods
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject);

	//IClassFactory methods
	HRESULT __stdcall CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	HRESULT __stdcall LockServer(BOOL fLock);
};