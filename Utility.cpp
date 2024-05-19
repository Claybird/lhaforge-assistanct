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

bool UtilCheckINISectionExists(const std::wstring& appName, const std::filesystem::path &file)
{
	auto path = std::filesystem::path(file).make_preferred();
	const size_t bufsize = 16;
	wchar_t szBuffer[bufsize] = {};
	DWORD dwRead = GetPrivateProfileSectionW(appName.c_str(), szBuffer, bufsize - 1, path.c_str());
	return dwRead>0;
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
