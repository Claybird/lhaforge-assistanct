#include "stdafx.h"
#include "resource.h"
#include "Utility.h"

#if defined(DEBUG) || defined(_DEBUG)
void UtilDebugTrace(LPCTSTR pszFormat, ...)
{
	va_list	args;

	va_start(args, pszFormat);
	CString str;
	str.FormatV(pszFormat,args);
	va_end(args);

	OutputDebugString(str);
}

void TraceLastError()
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
		(LPTSTR)&lpMsgBuf, 0, NULL);
	TRACE(_T("API Error : %s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
}

#endif

//エラーメッセージ表示
int ErrorMessage(LPCTSTR msg)
{
	TRACE(_T("ErrorMessage:")),TRACE(msg),TRACE(_T("\n"));
	return MessageBox(NULL,msg,UtilGetMessageCaption(),MB_OK|MB_ICONSTOP);
}

//メッセージキャプションを取得
LPCTSTR UtilGetMessageCaption()
{
	const static CString strCaption(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION));
	return strCaption;
}

void UtilGetLastErrorMessage(CString &strMsg)
{
	//実行エラー
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	strMsg=(LPCTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
}



//コマンドライン引数を取得(個数を返す)
int UtilGetCommandLineParams(std::vector<CString> &rParams)
{
	int nArgc=0;
	LPWSTR *lplpArgs=CommandLineToArgvW(GetCommandLine(), &nArgc);
	rParams.resize(nArgc);
	for(int i=0;i<nArgc;i++){
		rParams[i]=lplpArgs[i];
	}
	LocalFree(lplpArgs);
	return nArgc;
}


//INIに数字を文字列として書き込む
BOOL UtilWritePrivateProfileInt(LPCTSTR lpAppName,LPCTSTR lpKeyName,LONG nData,LPCTSTR lpFileName)
{
	TCHAR Buffer[32]={0};
	wsprintf(Buffer,_T("%ld"),nData);
	return ::WritePrivateProfileString(lpAppName,lpKeyName,Buffer,lpFileName);
}


//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}

//フルパスかつ絶対パスの取得
PATHERROR UtilGetCompletePathName(CString &_FullPath,LPCTSTR lpszFileName)
{
	ASSERT(lpszFileName&&_tcslen(lpszFileName)>0);
	if(!lpszFileName||_tcslen(lpszFileName)<=0){
		TRACE(_T("ファイル名が指定されていない\n"));
		return PATHERROR_INVALID;
	}

	//---絶対パス取得
	TCHAR szAbsPath[_MAX_PATH+1]={0};
	{
		TCHAR Buffer[_MAX_PATH+1]={0};	//ルートかどうかのチェックを行う
		_tcsncpy_s(Buffer,lpszFileName,_MAX_PATH);
		PathAddBackslash(Buffer);
		if(PathIsRoot(Buffer)){
			//ドライブ名だけが指定されている場合、
			//_tfullpathはそのドライブのカレントディレクトリを取得してしまう
			_tcsncpy_s(szAbsPath,lpszFileName,_MAX_PATH);
		}
		else if(!_tfullpath(szAbsPath,lpszFileName,_MAX_PATH)){
			TRACE(_T("絶対パス取得失敗\n"));
			return PATHERROR_ABSPATH;
		}
	}

	if(!PathFileExists(szAbsPath)&&!PathIsDirectory(szAbsPath)){
		//パスがファイルもしくはディレクトリとして存在しないなら、エラーとする
		TRACE(_T("ファイルが存在しない\n"));
		return PATHERROR_NOTFOUND;
	}
	if(!GetLongPathName(szAbsPath,szAbsPath,_MAX_PATH)){
		TRACE(_T("ロングファイル名取得失敗\n"));
		return PATHERROR_LONGNAME;
	}
	_FullPath=szAbsPath;
	return PATHERROR_NONE;
}

//自分のプログラムのおいてあるディレクトリのパス名を返す
LPCTSTR UtilGetModuleDirectoryPath()
{
	static TCHAR s_szDirPath[_MAX_PATH+1]={0};
	if(s_szDirPath[0]!=_T('\0'))return s_szDirPath;

	GetModuleFileName(GetModuleHandle(NULL), s_szDirPath, _MAX_PATH);	//本体のパス取得
	PathRemoveFileSpec(s_szDirPath);
	return s_szDirPath;
}
