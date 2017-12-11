#pragma once
#include <ShlObj.h>
#include <queue>

extern UINT g_cObjCount;

class ContextMenuHandler : public IShellExtInit, IContextMenu
{
protected:
	DWORD m_objRefCount;
	std::priority_queue<std::string> selectedFiles;
	~ContextMenuHandler();
public:
	ContextMenuHandler();
	//IUnknown
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject);

	//IShellExtInit
	HRESULT Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID);

	//IContextMenu

	HRESULT GetCommandString(UINT_PTR idCmd, UINT  uFlags, UINT *pwReserved,	LPSTR pszName, UINT cchMax);

	HRESULT InvokeCommand(LPCMINVOKECOMMANDINFO pici);

	HRESULT QueryContextMenu(HMENU hmenu, UINT  indexMenu, UINT  idCmdFirst, UINT  idCmdLast, UINT  uFlags);

};