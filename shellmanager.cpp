#include "stdafx.h"
#include "shellmanager.h"
#include "resource.h"



//-------------------------------------------------------------------------
// ShellRegisterServer			�g���V�F����o�^
//-------------------------------------------------------------------------
bool ShellRegisterServer(HWND hWnd,LPCTSTR inDllPath)
{
	// ���C�u���������[�h
	HINSTANCE theDllH = ::LoadLibrary(inDllPath);
	if(!theDllH){
		// ���[�h�o���܂���ł���
		CString msg;
		msg.Format(IDS_ERROR_DLL_LOAD,inDllPath);
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		return false;
	}

	// �G���g���[�|�C���g��T���Ď��s
	FARPROC lpDllEntryPoint;
	(FARPROC&)lpDllEntryPoint = ::GetProcAddress(theDllH,"DllRegisterServer");
	if(lpDllEntryPoint){
		// �o�^
		(*lpDllEntryPoint)();
//		MessageBeep(MB_OK);
	}else{
		// DllRegisterServer ��������܂���
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
// ShellUnregisterServer			�g���V�F��������
//-------------------------------------------------------------------------
bool ShellUnregisterServer(HWND hWnd,LPCTSTR inDllPath)
{
	// ���C�u���������[�h
	HINSTANCE theDllH = ::LoadLibrary(inDllPath);
	if(!theDllH){
		// ۰�ޏo���܂���ł���
		CString msg;
		msg.Format(IDS_ERROR_DLL_LOAD,inDllPath);
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		return false;
	}

	// �G���g���[�|�C���g��T���Ď��s
	FARPROC	lpDllEntryPoint;
	(FARPROC&)lpDllEntryPoint = ::GetProcAddress(theDllH,"DllUnregisterServer");
	if(lpDllEntryPoint){
		// �o�^
		(*lpDllEntryPoint)();
//		MessageBeep(MB_OK);
	}else{
		// DllUnregisterServer ��������܂���
		CString msg;
		msg.Format(IDS_ERROR_DLL_FUNCTION_GET,inDllPath,_T("DllUnregisterServer"));
		MessageBox(hWnd,msg,CString(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION)),MB_OK|MB_ICONSTOP);
		::FreeLibrary(theDllH);
		return false;
	}
	::FreeLibrary(theDllH);
	return true;
}

