#include "stdafx.h"

#include "Association.h"

//関連付けを作成
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
		//キーがオープン出来ない場合
		Ret = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, AssocInfo.Ext, NULL, NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKey,&dwDust);
		if(ERROR_SUCCESS!=Ret){
			//キーが作成出来ない場合
			TRACE(_T("AssocSetAssociation():Failed to Create Key %s\n"),AssocInfo.Ext);
			return false;
		}
	}
	//拡張子に対応するFileType設定
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
	// FileTypeの作成とアイコン情報設定
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
	// Shell\Openのコマンドを設定
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

//関連付けを削除
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

	//.???を開いて調査
	HKEY hKey;
	LONG Ret = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, Ext, NULL, KEY_ALL_ACCESS, &hKey);
	if(ERROR_SUCCESS!=Ret){
		TRACE(_T("AssocDeleteAssociation():Failed to Open %s\n"),Ext);
		return false;
	}
	TRACE(_T("AssocDeleteAssociation():Succeeded to Open %s\n"),Ext);

	//LhaForgeOrgFileTypeが読み出せれば、書かれているFileTypeがまだ存在するなら復元
	//FileTypeが存在せず、無効となっていれば無視し、どちらの場合にもLhaForgeOrgFileTypeは消す。
	std::vector<BYTE> Buffer;
	DWORD dwRead=0;
	::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL,&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query dwRead done\n"));
	Buffer.assign(dwRead+1,0);
	Ret = ::RegQueryValueEx(hKey, NULL, NULL, NULL, &Buffer.at(0),&dwRead);
	TRACE(_T("AssocDeleteAssociation():Query Value done\n"));
	CString TextBuffer=(TCHAR*)&Buffer.at(0);
	if(0!=TextBuffer.Left(_tcslen(ASSOC_PREFIX)).CompareNoCase(ASSOC_PREFIX)){
		//関連付けが自分の物でなければ、LhaForgeOrgFileTypeを消して静かに去る
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
		//存在しないと見なす
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
//関連付けを取得
//Extの情報を元に、FileTypeとそのIconについて調べる
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
	//拡張子を小文字で統一する
	AssocInfo.Ext.MakeLower();
	//拡張子に対応するFileType取得
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

	//オリジナルのFileType
	/*
	1.FileTypeが無いとき、OrgFIleType=.???\OrgFileType(NULLにもなりうる)
	2.FileTypeがLhaForgeArchive_*以外の時、OrgFileType=.???\(Default)
	3.FileTypeがLhaForgeArchive_*のとき、OrgFileType=.???\OrgFileType
	*/
	//1.と3.のとき
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
	else{	//2.のとき
		AssocInfo.OrgFileType=FileType;
		::RegCloseKey(hKey);
	}

	//------------------------------
	// FileTypeからアイコン情報取得
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
	//アイコン情報の取得
	AssocInfo.IconFile=(TCHAR*)&Buffer.at(0);
	AssocInfo.IconIndex=PathParseIconLocation(AssocInfo.IconFile.GetBuffer(_MAX_PATH*2));
	AssocInfo.IconFile.ReleaseBuffer();
	TRACE(_T("***%d:::%s\n"),AssocInfo.IconIndex,AssocInfo.IconFile);

	AssocInfo.OrgIconFile=AssocInfo.IconFile;
	AssocInfo.OrgIconIndex=AssocInfo.IconIndex;

	//----------------------------
	// Shell\Openのコマンドを読む
	//----------------------------
	if(!AssocInfo.bOrgStatus){
		//関連付けされていない
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

// 再起的レジストリ削除
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
