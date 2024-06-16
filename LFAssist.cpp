#include "stdafx.h"
#include "LFAssist.h"
#include "Utility.h"

void processShellExt(const CSimpleIniW& ini);
void processAssoc(const CSimpleIniW& ini);

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
	if (std::filesystem::is_regular_file(iniName)) {
		CSimpleIniW ini;
		ini.LoadFile(iniName.c_str());

		processShellExt(ini);
		processAssoc(ini);

		//delete ini if it contains specific keyword
		if (L"Please_Delete_Me" == ini.GetValue(L"PostProcess", L"DeleteMe")) {
			std::filesystem::remove(iniName);
		}

		//notify shell
		::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	}
	return 0;
}

