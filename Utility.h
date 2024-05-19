#pragma once

std::vector<std::wstring> UtilGetCommandLineArgs();

bool UtilCheckINISectionExists(const std::wstring& appName, const std::filesystem::path& file);

template <typename mapclass,typename keyclass>
bool has_key(const mapclass &theMap,keyclass theKey){
	return theMap.find(theKey)!=theMap.end();
}

std::filesystem::path UtilGetModulePath();
std::filesystem::path UtilGetModuleDirectoryPath();

