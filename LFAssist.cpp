// LFAssist.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "LFAssist.h"
#include "ShellManager.h"
#include "Association.h"
#include "Utility.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//�R�}���h���C�������̕���
	std::vector<CString> cmdParams;
	int nArgs=UtilGetCommandLineParams(cmdParams);

	if(nArgs<2){
		MessageBox(NULL,_T("Assistant program for LhaForge"),_T("Information"),MB_OK|MB_ICONINFORMATION);
		return 0;	//nothing to do
	}

	//�t�@�C�������擾
	CString strIniName;
	if(PATHERROR_NONE!=UtilGetCompletePathName(strIniName,cmdParams[1]))return 1;		//�����Ώۂ�������Ȃ�

	//---INI���

	LPCTSTR extArray[]={	//�֘A�Â��ΏۂƂȂ�g���q�̈ꗗ;�Z�N�V������͂��ʓ|�Ȃ̂ŁB
		_T(".lzh"),
		_T(".lzs"),
		_T(".lha"),
		_T(".zip"),
		_T(".jar"),
		_T(".cab"),
		_T(".7z"),
		_T(".arj"),
		_T(".rar"),
		_T(".jak"),
		_T(".gca"),
		_T(".imp"),
		_T(".ace"),
		_T(".yz1"),
		_T(".hki"),
		_T(".bza"),
		_T(".gza"),
		_T(".ish"),
		_T(".uue"),
		_T(".bel"),
		_T(".tar"),
		_T(".gz"),
		_T(".tgz"),
		_T(".bz2"),
		_T(".tbz"),
		_T(".z"),
		_T(".taz"),
		_T(".cpio"),
		_T(".a"),
		_T(".lib"),
		_T(".rpm"),
		_T(".deb"),
		_T(".iso"),
		_T(".lzma"),
		_T(".xz"),
		_T(".tlz"),
		_T(".txz"),
	};
	const UINT extCount=sizeof(extArray)/sizeof(extArray[0]);

	//�֘A�Â��̏���
	bool bChanged=false;	//�ݒ肪�ύX������true
	CString strCmd;	//�֘A�Â��R�}���h
	{
		CPath tmp(UtilGetModuleDirectoryPath());	//�{�̂̃p�X�擾
		tmp+=_T("LhaForge.exe");
		tmp.QuoteSpaces();
		strCmd=(CString)tmp;
		strCmd+=_T(" /m \"%1\"");	//�p�����[�^
	}

	for(UINT i=0;i<extCount;i++){
		//�Z�N�V���������݂��邩�ǂ����m�F
		TRACE(_T("Processing Assoc:%s\n"),extArray[i]);
		if(UtilCheckINISectionExists(extArray[i],strIniName)){
			TRACE(_T("Section found\n"));
			int nAction=GetPrivateProfileInt(extArray[i],_T("set"),-1,strIniName);
			if(0==nAction){	//�֘A�Â��폜
				AssocDeleteAssociation(extArray[i]);
			}else if(1==nAction){	//�V�K�Ɋ֘A�Â��쐬
				//�Â��֘A�Â��ݒ���擾
				ASSOCINFO ai;
				ai.Ext=extArray[i];
				AssocGetAssociation(ai);

				//�֘A�Â��R�}���h
				//GetPrivateProfileString(extArray[i],_T("cmd"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
				ai.ShellOpenCommand=strCmd;

				//�A�C�R�����
				TCHAR szBuffer[_MAX_PATH*2]={0};
				GetPrivateProfileString(extArray[i],_T("iconfile"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
				ai.IconFile=szBuffer;
				ai.IconIndex=GetPrivateProfileInt(extArray[i],_T("iconindex"),0,strIniName);

				//�ݒ���s
				AssocSetAssociation(ai);

				bChanged=true;
			}
		}
	}
	if(bChanged){
		::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSH,NULL,NULL);	//�֘A�t���̕ύX���V�F���ɒʒm
	}

	//�V�F���g���̏���
	if(UtilCheckINISectionExists(_T("Shell"),strIniName)){
		//DLL�̃p�X
		CPath tmp(UtilGetModuleDirectoryPath());
#ifdef WIN64
		tmp+=_T("ShellExtDLL64.dll");
#else
		tmp+=_T("ShellExtDLL.dll");
#endif

		//�������e
		int nAction=GetPrivateProfileInt(_T("Shell"),_T("set"),-1,strIniName);

		if(0==nAction){
			//����
			ShellUnregisterServer(NULL,tmp);
		}else if(1==nAction){
			//�Z�b�g
			ShellRegisterServer(NULL,tmp);
		}

		//�ύX���V�X�e���ɒʒm
		//::SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0L,NULL,SMTO_NORMAL,1000,NULL);
	}

	//�ύX���V�X�e���ɒʒm(���\��)
//	::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_FLUSH,NULL,NULL);

	{
		//�폜���邩�ǂ���;�둀��ɂ��Ӑ}���Ȃ��폜��h�����߁A����̃L�[�t���[�Y���܂܂��ꍇ�̂ݍ폜���s���悤�ɂ���
		TCHAR szBuffer[_MAX_PATH*2]={0};
		GetPrivateProfileString(_T("PostProcess"),_T("DeleteMe"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
		const CString strKeyPhrase(_T("Please_Delete_Me"));
		if(szBuffer==strKeyPhrase){
			::DeleteFile(strIniName);
		}
	}

	return 0;
}

