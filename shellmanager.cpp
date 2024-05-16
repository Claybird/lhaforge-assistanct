#include "stdafx.h"
#include "shellmanager.h"
#include "resource.h"



//-------------------------------------------------------------------------
// ShellRegisterServer			拡張シェルを登録
//-------------------------------------------------------------------------
bool ShellRegisterServer(HWND hWnd,LPCTSTR inDllPath)
{
	// ライブラリをロード
	HINSTANCE theDllH = ::LoadLibrary(inDllPath);
	if(!theDllH){
		// ロード出来ませんでした
		CString msg;
		msg.Format(IDS_ERROR_DLL_LOAD,inDllPath);
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		return false;
	}

	// エントリーポイントを探して実行
	FARPROC lpDllEntryPoint;
	(FARPROC&)lpDllEntryPoint = ::GetProcAddress(theDllH,"DllRegisterServer");
	if(lpDllEntryPoint){
		// 登録
		(*lpDllEntryPoint)();
//		MessageBeep(MB_OK);
	}else{
		// DllRegisterServer が見つかりません
		CString msg;
		msg.Format(IDS_ERROR_DLL_FUNCTION_GET,inDllPath,_T("DllRegisterServer"));
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		::FreeLibrary(theDllH);
		return false;
	}
	::FreeLibrary(theDllH);
	return true;
}

//-------------------------------------------------------------------------
// ShellUnregisterServer			拡張シェルを解除
//-------------------------------------------------------------------------
bool ShellUnregisterServer(HWND hWnd,LPCTSTR inDllPath)
{
	// ライブラリをロード
	HINSTANCE theDllH = ::LoadLibrary(inDllPath);
	if(!theDllH){
		// ﾛｰﾄﾞ出来ませんでした
		CString msg;
		msg.Format(IDS_ERROR_DLL_LOAD,inDllPath);
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		return false;
	}

	// エントリーポイントを探して実行
	FARPROC	lpDllEntryPoint;
	(FARPROC&)lpDllEntryPoint = ::GetProcAddress(theDllH,"DllUnregisterServer");
	if(lpDllEntryPoint){
		// 登録
		(*lpDllEntryPoint)();
//		MessageBeep(MB_OK);
	}else{
		// DllUnregisterServer が見つかりません
		CString msg;
		msg.Format(IDS_ERROR_DLL_FUNCTION_GET,inDllPath,_T("DllUnregisterServer"));
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		::FreeLibrary(theDllH);
		return false;
	}
	::FreeLibrary(theDllH);
	return true;
}

