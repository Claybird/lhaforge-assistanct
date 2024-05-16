//関連付け用API

#pragma once

const LPCTSTR ASSOC_PREFIX=_T("LhaForgeArchive_");


class ASSOCINFO{
public:
	CString Ext;	//拡張子;.付きの名前を渡すこと
//	CString FileType;
	CString OrgFileType;
	bool bOrgStatus;	//関連付けされていたらtrue

	CString OrgIconFile;
	int OrgIconIndex;

	CString IconFile;
	int IconIndex;

	CString ShellOpenCommand;
	ASSOCINFO():IconIndex(0),bOrgStatus(false){}
	virtual ~ASSOCINFO(){}
};

//関連付けを作成
bool AssocSetAssociation(const ASSOCINFO&);
//関連付けを削除
bool AssocDeleteAssociation(LPCTSTR Ext);
//関連付けを取得
bool AssocGetAssociation(ASSOCINFO&);

//レジストリ再帰的削除
bool AssocRecursiveDeleteKey(HKEY,LPCTSTR);
bool AssocRegExistKey(HKEY,LPCTSTR);

