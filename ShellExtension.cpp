// ShellExtension.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <Shlobj.h>
#include "GUID.h"
#include "ClassFactory.h"

#include <fstream>
#include "ThreadPool.h"
#include "FileHandler.h"


// Define a path for output
#define log_file "C:\\Logs\\log.txt"

UINT g_cObjCount;
HINSTANCE g_hInstance;
std::wstring dllName = L"ShellExtension.dll";
std::ofstream log_stream;


BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case (DLL_PROCESS_ATTACH):
			g_hInstance = hinstDLL;
			return true;
		default:
			break;
	}
	return true;
}

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	if (!IsEqualCLSID(rclsid, CLSID_ShellExtension)) return CLASS_E_CLASSNOTAVAILABLE;

	if (!ppv) return E_INVALIDARG;
	*ppv = NULL;

	HRESULT hr = E_UNEXPECTED;
	
	ClassFactory * pClassFactory = new ClassFactory();
	if (pClassFactory != NULL)
	{
		hr = pClassFactory->QueryInterface(riid, ppv);
		pClassFactory->Release();
	}

	if (!log_stream.is_open())
		log_stream.open(log_file, std::fstream::app);

	ThreadPool::Instance(&file_handler, &log_stream);

	return hr;
}

HRESULT __stdcall DllCanUnloadNow()
{
	HRESULT hr = E_UNEXPECTED;

	if (g_cObjCount > 0 || !ThreadPool::Instance().isReady())
	{
		hr = S_FALSE;
	}
	else
	{
		log_stream.close();
		hr = S_OK;
	}

	return hr;
}

std::wstring CompileStringFromCLSID(IID iid)
{
	wchar_t * tempString;
	StringFromCLSID(CLSID_ShellExtension, &tempString);
	return tempString;
}

std::wstring CompileGetModuleFileName()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(g_hInstance, buffer, MAX_PATH);
	return std::wstring(buffer);
}

DWORD MySizeInBytes(std::wstring target)
{
	DWORD result = DWORD((target.size() + 1 ) * 2);
	return result;
}

HRESULT __stdcall DllRegisterServer()
{
	HKEY hKey;
	DWORD lpDisp;
	std::wstring lpSubKey;

	lpSubKey = L"SOFTWARE\\Classes\\CLSID\\" + CompileStringFromCLSID(CLSID_ShellExtension);

	// create GUID key
	LONG result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &lpDisp);
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	// create InprocServer32 key
	result = RegCreateKeyEx(hKey, L"InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &lpDisp);
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	// set default value
	std::wstring lpDllPath;
	lpDllPath = CompileGetModuleFileName();
	result = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpDllPath.c_str(), MySizeInBytes(lpDllPath));
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	// create ThreadingModel and set value
	std::wstring apartment = L"Apartment";
	result = RegSetValueEx(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)apartment.c_str(), MySizeInBytes(apartment));
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	RegCloseKey(hKey);

	// create handler key
	lpSubKey = L"SOFTWARE\\Classes\\*\\ShellEx\\ContextMenuHandlers\\" + dllName;
	result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &lpDisp);
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	//set key default value
	result = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)CompileStringFromCLSID(CLSID_ShellExtension).c_str(), MySizeInBytes(CompileStringFromCLSID(CLSID_ShellExtension)));
	if (result != ERROR_SUCCESS) return E_UNEXPECTED;

	lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegSetValueEx(hKey, CompileStringFromCLSID(CLSID_ShellExtension).c_str(), 0, REG_SZ, (BYTE*)dllName.c_str(), MySizeInBytes(dllName));
		if (result != ERROR_SUCCESS) return E_UNEXPECTED;
	}

	RegCloseKey(hKey);

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return S_OK;
}
HRESULT __stdcall DllUnregisterServer()
{
	HKEY hKey;

	// check and delete InprocServer32 key if exists
	std::wstring lpSubKey = L"SOFTWARE\\Classes\\CLSID\\" + CompileStringFromCLSID(CLSID_ShellExtension) + L"\\InprocServer32";
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegDeleteKey(HKEY_LOCAL_MACHINE, lpSubKey.c_str());
		if (result != ERROR_SUCCESS) return E_UNEXPECTED;
	}

	// check and delete GUID key if exists
	lpSubKey = L"SOFTWARE\\Classes\\CLSID\\" + CompileStringFromCLSID(CLSID_ShellExtension);
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegDeleteKey(HKEY_LOCAL_MACHINE, lpSubKey.c_str());
		if (result != ERROR_SUCCESS) return E_UNEXPECTED;
	}
	RegCloseKey(hKey);

	// create handler key
	lpSubKey = L"SOFTWARE\\Classes\\*\\ShellEx\\ContextMenuHandlers\\" + dllName;
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegDeleteKey(HKEY_LOCAL_MACHINE, lpSubKey.c_str());
		if (result != ERROR_SUCCESS) return E_UNEXPECTED;
	}

	lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegDeleteValue(hKey, CompileStringFromCLSID(CLSID_ShellExtension).c_str());
		if (result != ERROR_SUCCESS) return E_UNEXPECTED;
	}
	RegCloseKey(hKey);

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return S_OK;
}
