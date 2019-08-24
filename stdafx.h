// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#define _WIN32_WINNT 0x0600
#define _ATL_NO_COM
#define _WTL_NO_UNION_CLASSES
#define _ATL_NO_MSIMG
#define _ATL_NO_OPENGL
//#define ATL_NO_LEAN_AND_MEAN
#define _ATL_USE_CSTRING_FLOAT	//CStringのFormatで小数が出力できるようになる

#define _STLP_USE_NEWALLOC	//STLで標準のアロケータを使う

//ATLのCStringを使う
//(cf.)http://hp.vector.co.jp/authors/VA022575/c/cstring.html
//#define _WTL_FORWARD_DECLARE_CSTRING
//#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _WTL_NO_CSTRING

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
//#include <atlcrack.h>
//#include <atlmisc.h>
//#include <atldlgs.h>
//#include <atlctrls.h>
//#include <atlframe.h>
#include <list>
#include <vector>
//#include <atlddx.h>		// DDX/DDVを使用するため
#if !defined(_UNICODE)&&!defined(UNICODE)
 #include <mbctype.h>
#endif//!defined(_UNICODE)&&!defined(UNICODE)
//#include <atlframe.h>
//#include <atlsplit.h>
//#include <atlctrlx.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <hash_map>
#include <hash_set>

#include <atlpath.h>
#include <shlobj.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


//文字型の定義
#if defined(_UNICODE)||defined(UNICODE)
typedef std::wstring stdString;
#else//defined(_UNICODE)||defined(UNICODE)
typedef std::string stdString;
#endif//defined(_UNICODE)||defined(UNICODE)

//個数を数えるマクロ
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))

#define FILL_ZERO(x)	::ZeroMemory(&x,sizeof(x))
#define ASSERT(x)	assert(x)

//TRACE
void UtilDebugTrace(LPCTSTR pszFormat, ...);
#if defined(_DEBUG) || defined(DEBUG)
#define TRACE UtilDebugTrace
#else	// Releaseのとき
#define TRACE
#endif	//_DEBUG
