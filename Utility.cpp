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
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
		(LPTSTR)&lpMsgBuf, 0, NULL);
	TRACE(_T("API Error : %s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
}

#endif

//�G���[���b�Z�[�W�\��
int ErrorMessage(LPCTSTR msg)
{
	TRACE(_T("ErrorMessage:")),TRACE(msg),TRACE(_T("\n"));
	return MessageBox(NULL,msg,UtilGetMessageCaption(),MB_OK|MB_ICONSTOP);
}

//���b�Z�[�W�L���v�V�������擾
LPCTSTR UtilGetMessageCaption()
{
	const static CString strCaption(MAKEINTRESOURCE(IDS_MESSAGE_CAPTION));
	return strCaption;
}

void UtilGetLastErrorMessage(CString &strMsg)
{
	//���s�G���[
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	strMsg=(LPCTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
}



//�R�}���h���C���������擾(����Ԃ�)
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


//INI�ɐ����𕶎���Ƃ��ď�������
BOOL UtilWritePrivateProfileInt(LPCTSTR lpAppName,LPCTSTR lpKeyName,LONG nData,LPCTSTR lpFileName)
{
	TCHAR Buffer[32]={0};
	wsprintf(Buffer,_T("%ld"),nData);
	return ::WritePrivateProfileString(lpAppName,lpKeyName,Buffer,lpFileName);
}


//INI�Ɏw�肳�ꂽ�Z�N�V����������Ȃ�true��Ԃ�
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}

//�t���p�X����΃p�X�̎擾
PATHERROR UtilGetCompletePathName(CString &_FullPath,LPCTSTR lpszFileName)
{
	ASSERT(lpszFileName&&_tcslen(lpszFileName)>0);
	if(!lpszFileName||_tcslen(lpszFileName)<=0){
		TRACE(_T("�t�@�C�������w�肳��Ă��Ȃ�\n"));
		return PATHERROR_INVALID;
	}

	//---��΃p�X�擾
	TCHAR szAbsPath[_MAX_PATH+1]={0};
	{
		TCHAR Buffer[_MAX_PATH+1]={0};	//���[�g���ǂ����̃`�F�b�N���s��
		_tcsncpy_s(Buffer,lpszFileName,_MAX_PATH);
		PathAddBackslash(Buffer);
		if(PathIsRoot(Buffer)){
			//�h���C�u���������w�肳��Ă���ꍇ�A
			//_tfullpath�͂��̃h���C�u�̃J�����g�f�B���N�g�����擾���Ă��܂�
			_tcsncpy_s(szAbsPath,lpszFileName,_MAX_PATH);
		}
		else if(!_tfullpath(szAbsPath,lpszFileName,_MAX_PATH)){
			TRACE(_T("��΃p�X�擾���s\n"));
			return PATHERROR_ABSPATH;
		}
	}

	if(!PathFileExists(szAbsPath)&&!PathIsDirectory(szAbsPath)){
		//�p�X���t�@�C���������̓f�B���N�g���Ƃ��đ��݂��Ȃ��Ȃ�A�G���[�Ƃ���
		TRACE(_T("�t�@�C�������݂��Ȃ�\n"));
		return PATHERROR_NOTFOUND;
	}
	if(!GetLongPathName(szAbsPath,szAbsPath,_MAX_PATH)){
		TRACE(_T("�����O�t�@�C�����擾���s\n"));
		return PATHERROR_LONGNAME;
	}
	_FullPath=szAbsPath;
	return PATHERROR_NONE;
}

//�����̃v���O�����̂����Ă���f�B���N�g���̃p�X����Ԃ�
LPCTSTR UtilGetModuleDirectoryPath()
{
	static TCHAR s_szDirPath[_MAX_PATH+1]={0};
	if(s_szDirPath[0]!=_T('\0'))return s_szDirPath;

	GetModuleFileName(GetModuleHandle(NULL), s_szDirPath, _MAX_PATH);	//�{�̂̃p�X�擾
	PathRemoveFileSpec(s_szDirPath);
	return s_szDirPath;
}
