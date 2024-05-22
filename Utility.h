#pragma once

std::vector<std::wstring> UtilGetCommandLineArgs();

bool UtilCheckINISectionExists(const std::wstring& appName, const std::filesystem::path& file);
std::wstring UtilRegQueryString(HKEY hKey, const std::wstring& path);
LSTATUS UtilRegSetKeyAndValue(HKEY root, const std::wstring& keyPath, const std::wstring& name, const std::wstring& value);

template <typename mapclass,typename keyclass>
bool has_key(const mapclass &theMap,keyclass theKey){
	return theMap.find(theKey)!=theMap.end();
}

std::filesystem::path UtilGetModulePath();
std::filesystem::path UtilGetModuleDirectoryPath();

inline std::wstring toUpper(const std::wstring& input) {
	std::wstring output;
	std::transform(input.begin(), input.end(), std::back_inserter(output),
		[](wchar_t c) {
		return towupper(c);
	});
	return output;
}
