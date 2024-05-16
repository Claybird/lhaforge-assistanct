#include "stdafx.h"

#include "Association.h"

//�֘A�t�����쐬
bool AssocSetAssociation(const ASSOCINFO& AssocInfo)
{
	TRACE(_T("AssocSetAsscociation()\n---"));
	TRACE(_T("Ext=%s\n"),AssocInfo.Ext);
//	TRACE(_T("FileType=%s\n"),AssocInfo.FileType);
	TRACE(_T("IconFile=%s\nIndex=%d\n"),AssocInfo.IconFile,AssocInfo.IconIndex);
	TRACE(_T("ShellOpenCommand=%s\n---\n"),AssocInfo.ShellOpenCommand);

	HKEY hKey;
	LONG Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, AssocInfo.Ext, NULL,KEY_ALL_ACCESS,&hKey);
	DWORD dwDust;
	if(ERROR_SUCCESS!=Ret){
		//�L�[���I�[�v���o���Ȃ��ꍇ
		Ret = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, AssocInfo.Ext, NULL, NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey,&dwDust);
		if(ERROR_SUCCESS!=Ret){
			//�L�[���쐬�o���Ȃ��ꍇ
			TRACE(_T("AssocSetAssociation():Failed to Create Key %s\n"),AssocInfo.Ext);
			return false;
		}
	}
	//�g���q�ɑΉ�����FileType�ݒ�
	CString FileType=ASSOC_PREFIX;
	{
		CString ExtBuffer=AssocInfo.Ext;
		ExtBuffer.Delete(0);
		ExtBuffer.MakeUpper();
		FileType+=ExtBuffer;
	}

	DWORD dwWrite=(FileType.GetLength()+1)*sizeof(TCHAR);
	Ret = ::RegSetValueEx(hKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)FileType,dwWrite);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocSetAssociation():Failed to Set FileType\n"));
		::RegCloseKey(hKey);
		return false;
	}

	//OrgFileType
	if(AssocInfo.OrgFileType.IsEmpty()){
		::RegDeleteValue(hKey, _T("LhaForgeOrgFileType"));
	}
	else{
		dwWrite=(AssocInfo.OrgFileType.GetLength()+1)*sizeof(TCHAR);
		Ret = ::RegSetValueEx(hKey, _T("LhaForgeOrgFileType"), NULL, REG_SZ, (BYTE*)(LPCTSTR)AssocInfo.OrgFileType,dwWrite);
		if(ERROR_SUCCESS!=Ret){
			TRACE(_T("AssocSetAssociation():Failed to Set OrgFileType\n"));
			::RegCloseKey(hKey);
			return false;
		}
	}
	::RegCloseKey(hKey);

	//----------------------------------
	// FileType�̍쐬�ƃA�C�R�����ݒ�
	//----------------------------------
	CString KeyNameBuffer=FileType;
	KeyNameBuffer+=_T("\\DefaultIcon");
	Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL, KEY_ALL_ACCESS,&hKey);
	if(ERROR_SUCCESS!=Ret){
		Ret = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL, NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey,&dwDust);
		if(ERROR_SUCCESS!=Ret){
			TRACE(_T("AssocSetAssociation():Failed to Create %s\n"),KeyNameBuffer);
			return false;
		}
	}

	CString IconFile;
	IconFile.Format(_T("%s,%d"),AssocInfo.IconFile,AssocInfo.IconIndex);
	dwWrite=(IconFile.GetLength()+1)*sizeof(TCHAR);
	Ret = ::RegSetValueEx(hKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)IconFile,dwWrite);
	::RegCloseKey(hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocSetAssociation():Failed to Set IconInfo\n"));
		return false;
	}

	//----------------------------
	// Shell\Open�̃R�}���h��ݒ�
	//----------------------------
	KeyNameBuffer=FileType;
	KeyNameBuffer+=_T("\\shell\\open\\command");
	Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL,KEY_ALL_ACCESS,&hKey);
	if(ERROR_SUCCESS!=Ret){
		Ret = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL, NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey,&dwDust);
		if(ERROR_SUCCESS!=Ret){
			TRACE(_T("AssocSetAssociation():Failed to Create %s\n"),KeyNameBuffer);
			return false;
		}
	}

	dwWrite=(AssocInfo.ShellOpenCommand.GetLength()+1)*sizeof(TCHAR);
	Ret = ::RegSetValueEx(hKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)AssocInfo.ShellOpenCommand,dwWrite);
	::RegCloseKey(hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocSetAssociation():Failed to Set ShellOpenCommand\n"));
		return false;
	}

	return true;
}

//�֘A�t�����폜
bool AssocDeleteAssociation(LPCTSTR Ext)
{
	TRACE(_T("AssocDeleteAsscociation()---\n"));
	TRACE(_T("Ext=%s\n"),Ext);

	CString FileType=ASSOC_PREFIX;
	{
		CString ExtBuffer=Ext;
		ExtBuffer.Delete(0);
		ExtBuffer.MakeUpper();
		FileType+=ExtBuffer;
	}
	AssocRecursiveDeleteKey(HKEY_CLASSES_ROOT,FileType);
	TRACE(_T("%s Deleted.\n"),FileType);

	//.???���J���Ē���
	HKEY hKey;
	LONG Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, Ext, NULL, KEY_ALL_ACCESS, &hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocDeleteAssociation():Failed to Open %s\n"),Ext);
		return false;
	}
	TRACE(_T("AssocDeleteAssociation():Succeeded to Open %s\n"),Ext);

	//LhaForgeOrgFileType���ǂݏo����΁A������Ă���FileType���܂����݂���Ȃ畜��
	//FileType�����݂����A�����ƂȂ��Ă���Ζ������A�ǂ���̏ꍇ�ɂ�LhaForgeOrgFileType�͏����B
	std::vector<BYTE> Buffer;
	DWORD dwRead=0;
	::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL,&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query dwRead done\n"));
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, NULL, NULL, NULL, &Buffer.at(0),&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query Value done\n"));
	CString TextBuffer=(TCHAR*)&Buffer.at(0);
	if(0!=TextBuffer.Left(_tcslen(ASSOC_PREFIX)).CompareNoCase(ASSOC_PREFIX)){
		//�֘A�t���������̕��łȂ���΁ALhaForgeOrgFileType�������ĐÂ��ɋ���
		::RegDeleteValue(hKey,_T("LhaForgeOrgFileType"));
		::RegCloseKey(hKey);
		TRACE(_T("LhaForgeOrgFileType Deleted.\n"));
		return true;
	}
	::RegQueryValueEx(hKey, _T("LhaForgeOrgFileType"), NULL, NULL, NULL,&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query LhaForgeOrgFileType dwRead done\n"));
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, _T("LhaForgeOrgFileType"), NULL, NULL, &Buffer.at(0),&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query LhaForgeOrgFileType done\n"));
	TextBuffer=(TCHAR*)&Buffer.at(0);
	if(ERROR_SUCCESS!=Ret||TextBuffer.IsEmpty()){
		//���݂��Ȃ��ƌ��Ȃ�
		TRACE(_T("AssocDeleteAssociation():Failed to Get OrgFileType\n"));
	}
	else{
		if(AssocRegExistKey(HKEY_CLASSES_ROOT,TextBuffer)){
			TRACE(_T("AssocDeleteAssociation():%s exists\n"),TextBuffer);
			Ret=::RegSetValueEx(hKey, NULL, NULL, REG_SZ, &Buffer.at(0),dwRead);
			if(ERROR_SUCCESS!=Ret){
				TRACE(_T("AssocDeleteAssociation():Failed to Set Old FileType\n"),TextBuffer);
			}
		}
		::RegDeleteValue(hKey,_T("LhaForgeOrgFileType"));
		TRACE(_T("LhaForgeOrgFileType Deleted.\n"));
		::RegCloseKey(hKey);
		return true;
	}
	::RegDeleteValue(hKey,NULL);

	DWORD dwKeyCount=0;
	Ret=::RegQueryInfoKey(hKey,NULL,NULL,NULL,&dwKeyCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	if(ERROR_SUCCESS==Ret&&0==dwKeyCount){
		AssocRecursiveDeleteKey(HKEY_CLASSES_ROOT,Ext);
		TRACE(_T("HKEY_CLASSES_ROOT\\%s Deleted.\n"),Ext);
	}
	::RegCloseKey(hKey);

	return false;
}

#ifndef __UNINSTALLER__
//�֘A�t�����擾
//Ext�̏������ɁAFileType�Ƃ���Icon�ɂ��Ē��ׂ�
bool AssocGetAssociation(ASSOCINFO &AssocInfo)
{
	if(AssocInfo.Ext.IsEmpty()){
		TRACE(_T("AssocGetAssociation():Ext is Empty\n"));
		return false;
	}
	HKEY hKey;
	LONG Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, AssocInfo.Ext, NULL, KEY_READ, &hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocGetAssociation():Failed to Open '%s'\n"),AssocInfo.Ext);
		return false;
	}
	//�g���q���������œ��ꂷ��
	AssocInfo.Ext.MakeLower();
	//�g���q�ɑΉ�����FileType�擾
//	DWORD ValueType;
	std::vector<BYTE> Buffer;
	DWORD dwRead=0;
	::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL,&dwRead);
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, NULL, NULL, NULL, &Buffer.at(0),&dwRead);
	if(ERROR_SUCCESS!=Ret){
		::RegCloseKey(hKey);
		TRACE(_T("AssocGetAssociation():Failed to Get FileType\n"));
		return false;
	}
	CString FileType=(TCHAR*)&Buffer.at(0);

	//�I���W�i����FileType
	/*
	1.FileType�������Ƃ��AOrgFIleType=.???\OrgFileType(NULL�ɂ��Ȃ肤��)
	2.FileType��LhaForgeArchive_*�ȊO�̎��AOrgFileType=.???\(Default)
	3.FileType��LhaForgeArchive_*�̂Ƃ��AOrgFileType=.???\OrgFileType
	*/
	//1.��3.�̂Ƃ�
	if(0==FileType.Left(_tcslen(ASSOC_PREFIX)).CompareNoCase(ASSOC_PREFIX)){
		AssocInfo.bOrgStatus=true;
	}
	else{
		AssocInfo.bOrgStatus=false;
	}
	if(FileType.IsEmpty()||(0==FileType.Left(_tcslen(ASSOC_PREFIX)).CompareNoCase(ASSOC_PREFIX))){
		::RegQueryValueEx(hKey, _T("LhaForgeOrgFileType"), NULL, NULL, NULL,&dwRead);
		Buffer.assign(dwRead+1,0);
		Ret = ::RegQueryValueEx(hKey, _T("LhaForgeOrgFileType"), NULL, NULL, &Buffer.at(0),&dwRead);
		::RegCloseKey(hKey);
		if(ERROR_SUCCESS!=Ret){
			AssocInfo.OrgFileType.Empty();
			TRACE(_T("AssocGetAssociation():Failed to Get OrgFileType\n"));
		}
		else{
			AssocInfo.OrgFileType=(TCHAR*)&Buffer.at(0);
		}
	}
	else{	//2.�̂Ƃ�
		AssocInfo.OrgFileType=FileType;
		::RegCloseKey(hKey);
	}

	//------------------------------
	// FileType����A�C�R�����擾
	//------------------------------
	CString KeyNameBuffer=FileType;
	KeyNameBuffer+=_T("\\DefaultIcon");
	Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL, KEY_READ, &hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocGetAssociation('%s'):Failed to Open '%s' for Default Icon\n"),AssocInfo.Ext,KeyNameBuffer);
		return false;
	}

	dwRead=0;
	::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL,&dwRead);
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, NULL, NULL, NULL, &Buffer.at(0),&dwRead);
	::RegCloseKey(hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocGetAssociation():Failed to Get IconInfo\n"));
		return false;
	}
	//�A�C�R�����̎擾
	AssocInfo.IconFile=(TCHAR*)&Buffer.at(0);
	AssocInfo.IconIndex=PathParseIconLocation(AssocInfo.IconFile.GetBuffer(_MAX_PATH*2));
	AssocInfo.IconFile.ReleaseBuffer();
	TRACE(_T("***%d:::%s\n"),AssocInfo.IconIndex,AssocInfo.IconFile);

	AssocInfo.OrgIconFile=AssocInfo.IconFile;
	AssocInfo.OrgIconIndex=AssocInfo.IconIndex;

	//----------------------------
	// Shell\Open�̃R�}���h��ǂ�
	//----------------------------
	if(!AssocInfo.bOrgStatus){
		//�֘A�t������Ă��Ȃ�
		return true;
	}
	KeyNameBuffer=FileType;
	KeyNameBuffer+=_T("\\shell\\open\\command");
	Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, KeyNameBuffer, NULL, KEY_READ, &hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocGetAssociation():Failed to Open '%s' for Open Command\n"),KeyNameBuffer);
		return false;
	}

	dwRead=0;
	::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL,&dwRead);
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, NULL, NULL, /*&ValueType*/NULL, &Buffer.at(0),&dwRead);
	::RegCloseKey(hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocGetAssociation():Failed to Get ShellOpenCommand\n"));
		return false;
	}
	AssocInfo.ShellOpenCommand=(TCHAR*)&Buffer.at(0);

	return true;
}
#endif //__UNINSTALLER__

// �ċN�I���W�X�g���폜
bool AssocRecursiveDeleteKey(HKEY hParentKey,LPCTSTR KeyName)
{
	HKEY		hChildKey;
	LONG		Ret;
	DWORD		dwSize;
	TCHAR		Buffer[256];

	// Open the child.
	Ret = ::RegOpenKeyEx(hParentKey, KeyName, 0, KEY_ALL_ACCESS, &hChildKey);
	if(ERROR_SUCCESS != Ret){
		return false;
	}

	// Enumerate all of the decendents of this child.
	dwSize = 256;
	while(S_OK==::RegEnumKeyEx(hChildKey, 0, Buffer, &dwSize, NULL, NULL, NULL, NULL)){
		// Delete the decendents of this child.
		Ret = AssocRecursiveDeleteKey(hChildKey, Buffer);
		if(!Ret){
			// Cleanup before exiting.
			::RegCloseKey(hChildKey);
			return false;
		}
		dwSize = 256;
	}

	// Close the child.
	::RegCloseKey(hChildKey);

	// Delete this child.
	return ERROR_SUCCESS==RegDeleteKey(hParentKey, KeyName);
}

bool AssocRegExistKey(HKEY hParentKey,LPCTSTR KeyName)
{
	DWORD		dwSize;
	TCHAR		Buffer[256];

	// Enumerate all of the decendents of this child.
	dwSize = 256;
	int i=0;
	while(S_OK==::RegEnumKeyEx(hParentKey, i, Buffer, &dwSize, NULL, NULL, NULL, NULL)){
		TRACE(_T("%d is %s\n"),i,Buffer);
		if(0==_tcsncmp(Buffer,KeyName,256)){
			return true;
		}
		dwSize = 256;
		i++;
	}

	return false;
}
