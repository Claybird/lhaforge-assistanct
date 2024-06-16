#include "stdafx.h"
#include "resource.h"
#include "utility.h"

bool callDllProcSub(HWND hWnd, const std::filesystem::path& dllPath, const std::string& procname)
{
	HINSTANCE hInstance = ::LoadLibraryW(dllPath.c_str());
	if (!hInstance) {
		auto msg = Format(UtilLoadString(IDS_ERROR_DLL_LOAD), dllPath.c_str());
		::MessageBoxW(hWnd, msg.c_str(), UtilLoadString(IDS_MESSAGE_CAPTION).c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	FARPROC lpDllEntryPoint;
	(FARPROC&)lpDllEntryPoint = ::GetProcAddress(hInstance, procname.c_str());
	if (lpDllEntryPoint) {
		(*lpDllEntryPoint)();
	} else {
		auto msg = Format(UtilLoadString(IDS_ERROR_DLL_FUNCTION_GET), dllPath.c_str(), procname.c_str());
		::MessageBoxW(hWnd, msg.c_str(), UtilLoadString(IDS_MESSAGE_CAPTION).c_str(), MB_OK | MB_ICONSTOP);
		::FreeLibrary(hInstance);
		return false;
	}
	::FreeLibrary(hInstance);
	return true;
}

bool shellRegisterServer(HWND hWnd, const std::filesystem::path &dllPath)
{
	return callDllProcSub(hWnd, dllPath, "DllRegisterServer");
}

bool shellUnregisterServer(HWND hWnd, const std::filesystem::path& dllPath)
{
	return callDllProcSub(hWnd, dllPath, "DllUnregisterServer");
}

void processShellExt(const CSimpleIniW& ini)
{
	auto dllPath = UtilGetModuleDirectoryPath() / L"ShellExtDLL64.dll";

	int nAction = ini.GetLongValue(L"Shell", L"set", -1);

	if (0 == nAction) {
		shellUnregisterServer(NULL, dllPath);
	} else if (nAction > 0) {
		shellRegisterServer(NULL, dllPath);
	}
}

