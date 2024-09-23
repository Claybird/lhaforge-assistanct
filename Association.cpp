#include "stdafx.h"

#include "Utility.h"

const wchar_t* ASSOC_PREFIX = L"LhaForge2Archive_";

struct ASSOCINFO{
	std::wstring fileType;
	std::wstring orgFileType;
};


std::wstring extToFileType(const std::wstring& ext)
{
	std::wstring fileType = ASSOC_PREFIX + toUpper(ext.c_str() + 1);	//omit dot
	return fileType;
}

ASSOCINFO getAssociation(const std::wstring& ext)
{
	ASSOCINFO assoc;
	assoc.fileType = UtilRegQueryString(HKEY_CLASSES_ROOT, ext);
	assoc.orgFileType = UtilRegQueryString(HKEY_CLASSES_ROOT, ext + L"\\LhaForge2OrgFileType");
	return assoc;
}

void unsetAssociation(const std::wstring& ext)
{
	auto currentAssoc = getAssociation(ext);

	std::wstring fileType = extToFileType(ext);
	RegDeleteTreeW(HKEY_CLASSES_ROOT, fileType.c_str());

	if (currentAssoc.fileType == fileType) {
		if (currentAssoc.orgFileType.empty()) {
			UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, ext, L"", L"");
			RegDeleteKeyW(HKEY_CLASSES_ROOT, (ext + L"\\LhaForge2OrgFileType").c_str());
		} else {
			auto prevCmd = UtilRegQueryString(HKEY_CLASSES_ROOT, currentAssoc.orgFileType + L"\\shell\\open\\command");
			if (!prevCmd.empty()) {
				//previous association is still alive
				UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, ext, L"", currentAssoc.orgFileType.c_str());
			}
		}
	}
	//remove backup
	RegDeleteTreeW(HKEY_CLASSES_ROOT, (ext + L"\\LhaForge2OrgFileType").c_str());
}

void setAssociation(const std::wstring &ext, const std::wstring& cmd, const std::wstring &iconInfo)
{
	auto currentAssoc = getAssociation(ext);

	std::wstring fileType = extToFileType(ext);
	UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, ext, L"", fileType.c_str());
	//backup
	UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, ext, L"LhaForge2OrgFileType", currentAssoc.fileType);

	//set filetype
	//icon
	UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, fileType + L"\\DefaultIcon", L"", iconInfo);
	//open
	UtilRegSetKeyAndValue(HKEY_CLASSES_ROOT, fileType + L"\\shell\\open\\command", L"", cmd);
}

static const std::vector<wchar_t*> extArray = {
	L".lzh",
	L".lzs",
	L".lha",

	L".zip",
	L".cab",
	L".zipx",

	L".7z",

	L".rar",
	L".arj",
	L".bza",
	L".gza",

	L".uue",

	L".tar",
	L".gz",
	L".bz2",
	L".xz",
	L".lzma",
	L".zst",
	L".lz4",
	L".z",
	L".cpio",
	L".tgz",
	L".tbz",
	L".txz",
	L".tlz",
	L".taz",

	L".iso",
};

void processAssoc(const CSimpleIniW& ini)
{
	auto exe = UtilGetModuleDirectoryPath() / L"LhaForge.exe";
	std::wstring strCmd = L"\"" + exe.make_preferred().wstring() + L"\"";
	strCmd += L" /m \"%1\"";

	for (const auto& ext : extArray) {
		if (ini.KeyExists(ext, L"set")) {
			int nAction = ini.GetLongValue(ext, L"set", -1);
			if (0 == nAction) {
				unsetAssociation(ext);
			} else if (nAction > 0) {
				std::wstring iconFile = ini.GetValue(ext, L"iconfile");
				int iconIndex = ini.GetLongValue(ext, L"iconindex", 0);

				setAssociation(ext, strCmd, Format(L"%s,%d", iconFile, iconIndex));
			}
		}
	}
}

void unsetAssoc()
{
	for (const auto& ext : extArray) {
		unsetAssociation(ext);
	}
}

void setDefaultAssoc()
{
	auto exe = UtilGetModuleDirectoryPath() / L"LhaForge.exe";
	std::wstring strCmd = L"\"" + exe.make_preferred().wstring() + L"\"";
	strCmd += L" /m \"%1\"";

	for (const std::wstring ext : extArray) {
		if (L".iso" != ext) {
			std::filesystem::path iconFile = UtilGetModuleDirectoryPath() / (L"icons/archive" + ext + L".ico");
			int iconIndex = 0;

			setAssociation(ext, strCmd, Format(L"%s,%d", iconFile.make_preferred().wstring(), iconIndex));
		}
	}
}

