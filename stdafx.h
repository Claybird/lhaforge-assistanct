#pragma once

#define _WIN32_WINNT 0x0600


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>

#include <shlobj.h>
#include <simpleini.h>	//dependency/simpleini/simpleini.h

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")

