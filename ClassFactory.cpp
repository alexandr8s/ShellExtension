#include "stdafx.h"
#include "ClassFactory.h"
#include "ContextMenuHandler.h"

extern UINT g_cObjCount;

ClassFactory::ClassFactory(): m_objRefCount(1)
{
	InterlockedIncrement(&g_cObjCount);
}

ClassFactory::~ClassFactory()
{
	InterlockedDecrement(&g_cObjCount);
}

ULONG ClassFactory::AddRef()
{
	return InterlockedIncrement(&m_objRefCount);
}

ULONG ClassFactory::Release()
{
	ULONG returnValue = InterlockedDecrement(&m_objRefCount);
	if (returnValue < 1)
	{
		delete this;
	}
	return returnValue;
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void ** ppvObject)
{
	if (!ppvObject)
		return E_POINTER;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObject = this;
		this->AddRef();
		return S_OK;
	}
	else if (IsEqualIID(riid, IID_IClassFactory))
	{
		*ppvObject = (IClassFactory*)this;
		this->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

HRESULT ClassFactory::CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject)
{
	if (!ppvObject)
		return E_INVALIDARG;

	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	HRESULT hr;
	if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IContextMenu))
	{
		ContextMenuHandler *pContextMenuHandler = new ContextMenuHandler();
		if (pContextMenuHandler == NULL)
			return E_OUTOFMEMORY;

		hr = pContextMenuHandler->QueryInterface(riid, ppvObject);
			pContextMenuHandler->Release();
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT ClassFactory::LockServer(BOOL fLock)
{
	return S_OK;
}
