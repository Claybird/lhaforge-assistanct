#include "stdafx.h"
#include "resource.h"
#include "Utility.h"


std::vector<std::wstring> UtilGetCommandLineArgs()
{
	int nArgc=0;
	std::vector<std::wstring> args;
	LPWSTR *lplpArgs=CommandLineToArgvW(GetCommandLineW(), &nArgc);
	args.resize(nArgc);
	for (int i = 0; i < nArgc; i++) {
		args[i] = lplpArgs[i];
	}
	LocalFree(lplpArgs);
	return args;
}

std::wstring UtilRegQueryString(HKEY hKey, const std::wstring& path)
{
	LONG Ret = ::RegOpenKeyExW(hKey, path.c_str(), NULL, KEY_READ, &hKey);
	if (ERROR_SUCCESS != Ret) {
		return L"";
	}

	std::vector<BYTE> Buffer;
	DWORD dwRead = 0;
	::RegQueryValueExW(hKey, NULL, NULL, NULL, NULL, &dwRead);
	Buffer.resize(dwRead + 1, 0);
	Ret = ::RegQueryValueExW(hKey, NULL, NULL, NULL, &Buffer.at(0), &dwRead);
	::RegCloseKey(hKey);

	if (ERROR_SUCCESS == Ret) {
		return (const wchar_t*)&Buffer.at(0);
	} else {
		return L"";
	}
}

LSTATUS UtilRegSetKeyAndValue(HKEY root, const std::wstring& keyPath, const std::wstring& name, const std::wstring& value)
{
	auto result = RegSetKeyValueW(root,
		keyPath.c_str(),
		name.c_str(),
		REG_SZ,
		(const BYTE*)value.c_str(),
		(DWORD)(wcslen(value.c_str()) + 1) * sizeof(wchar_t));

	return result;
}

//executable name
std::filesystem::path UtilGetModulePath()
{
	std::wstring name;
	name.resize(256);
	for (;;) {
		DWORD bufsize = (DWORD)name.size();
		auto nCopied = GetModuleFileNameW(GetModuleHandleW(nullptr), &name[0], bufsize);
		if (nCopied < bufsize) {
			break;
		} else {
			name.resize(name.size() * 2);
		}
	}
	return name.c_str();
}

std::filesystem::path UtilGetModuleDirectoryPath()
{
	return std::filesystem::path(UtilGetModulePath()).parent_path();
}

//loads string from resource
std::wstring UtilLoadString(UINT uID)
{
	wchar_t buf[256];
	LoadStringW(GetModuleHandleW(nullptr), uID, buf, 256);
	return buf;
}
