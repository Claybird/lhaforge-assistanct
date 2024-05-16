#pragma once

#define _WIN32_WINNT 0x0600


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <list>
#include <vector>
#include <time.h>
#include <algorithm>
#include <string>

#include <shlobj.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#define ASSERT(x)	assert(x)

//TODO
#define TRACE
//TODO:remove
#include <atlpath.h>
