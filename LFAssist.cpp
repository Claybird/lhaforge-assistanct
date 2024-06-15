#include "stdafx.h"
#include "LFAssist.h"
#include "ShellManager.h"
#include "Association.h"
#include "Utility.h"

void processDeleteFile(const std::filesystem::path& iniName)
{
	//delete ini if it contains specific keyword
	wchar_t szBuffer[_MAX_PATH * 2] = { 0 };
	GetPrivateProfileStringW(L"PostProcess", L"DeleteMe", L"", szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]) - 1, iniName.c_str());
	const std::wstring expectedPhrase = L"Please_Delete_Me";
	if (szBuffer == expectedPhrase) {
		::DeleteFileW(iniName.c_str());
	}
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	auto cmdArgs = UtilGetCommandLineArgs();

	if(cmdArgs.size()<2){
		MessageBoxW(NULL, L"Assistant program for LhaForge", L"Information", MB_OK | MB_ICONINFORMATION);
		return 0;	//nothing to do
	}

	std::filesystem::path iniName = cmdArgs[1];

	processShellExt(iniName);
	processAssoc(iniName);

	processDeleteFile(iniName);

	//notify shell
	::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return 0;
}

