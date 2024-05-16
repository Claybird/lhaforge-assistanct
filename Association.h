//�֘A�t���pAPI

#pragma once

const LPCTSTR ASSOC_PREFIX=_T("LhaForgeArchive_");


class ASSOCINFO{
public:
	CString Ext;	//�g���q;.�t���̖��O��n������
//	CString FileType;
	CString OrgFileType;
	bool bOrgStatus;	//�֘A�t������Ă�����true

	CString OrgIconFile;
	int OrgIconIndex;

	CString IconFile;
	int IconIndex;

	CString ShellOpenCommand;
	ASSOCINFO():IconIndex(0),bOrgStatus(false){}
	virtual ~ASSOCINFO(){}
};

//�֘A�t�����쐬
bool AssocSetAssociation(const ASSOCINFO&);
//�֘A�t�����폜
bool AssocDeleteAssociation(LPCTSTR Ext);
//�֘A�t�����擾
bool AssocGetAssociation(ASSOCINFO&);

//���W�X�g���ċA�I�폜
bool AssocRecursiveDeleteKey(HKEY,LPCTSTR);
bool AssocRegExistKey(HKEY,LPCTSTR);

