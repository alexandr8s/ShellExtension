#include "stdafx.h"
#include <string>
#include <atlstr.h>
#include "ThreadPool.h"
#include "ContextMenuHandler.h"


ContextMenuHandler::~ContextMenuHandler()
{
	InterlockedDecrement(&g_cObjCount);
}

ContextMenuHandler::ContextMenuHandler(): m_objRefCount(1)
{
	InterlockedIncrement(&g_cObjCount);
}

ULONG ContextMenuHandler::AddRef()
{
	return InterlockedIncrement(&m_objRefCount);
}

ULONG ContextMenuHandler::Release()
{
	ULONG returnValue = InterlockedDecrement(&m_objRefCount);
	if (returnValue < 1)
	{
		delete this;
	}
	return returnValue;
}

HRESULT ContextMenuHandler::QueryInterface(REFIID riid, void ** ppvObject)
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
	else if (IsEqualIID(riid, IID_IContextMenu))
	{
		*ppvObject = (IContextMenu*)this;
		this->AddRef();
		return S_OK;
	}
	else if (IsEqualIID(riid, IID_IShellExtInit))
	{
		*ppvObject = (IShellExtInit*)this;
		this->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

HRESULT ContextMenuHandler::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject * pdtobj, HKEY hkeyProgID)
{
	HRESULT hr = E_INVALIDARG;
	if (NULL == pdtobj)
		return hr;

	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm = {};

	if (SUCCEEDED(pdtobj->GetData(&fe, &stm)))
	{
		HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
		if (hDrop != NULL)
		{
			UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFiles != 0)
			{
				for (UINT i = 0; i < nFiles; i++)
				{
					int size = DragQueryFile(hDrop, i, NULL, 0) + 1;
					std::wstring str;
					str.resize(size);
					if (DragQueryFile(hDrop, i, &str[0], size) == 0)
						continue;
					selectedFiles.emplace(CW2A(str.c_str()));
				}
				hr = S_OK;
			}
			GlobalUnlock(stm.hGlobal);
		}
		ReleaseStgMedium(&stm);
	}

	return hr;
}

HRESULT ContextMenuHandler::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT * pwReserved, LPSTR pszName, UINT cchMax)
{
	return E_NOTIMPL;
}

HRESULT ContextMenuHandler::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
	ThreadPool & tp = ThreadPool::Instance();
	tp.pushTasks(&selectedFiles);

	return S_OK;
}

HRESULT ContextMenuHandler::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	MENUITEMINFO ContextItem = {};
	ContextItem.cbSize = sizeof(MENUITEMINFO);
	ContextItem.dwTypeData = L"Calculate checksum";
	ContextItem.fMask = MIIM_STRING | MIIM_ID;
	ContextItem.wID = idCmdFirst;
	if (!InsertMenuItem(hmenu, 0, TRUE, &ContextItem))
		return HRESULT_FROM_WIN32(GetLastError());
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, ContextItem.wID - idCmdFirst + 1);
}
