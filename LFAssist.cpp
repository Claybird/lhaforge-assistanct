// LFAssist.cpp : アプリケーションのエントリ ポイントを定義します。
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

	//コマンドライン引数の分析
	std::vector<CString> cmdParams;
	int nArgs=UtilGetCommandLineParams(cmdParams);

	if(nArgs<2){
		MessageBox(NULL,_T("Assistant program for LhaForge"),_T("Information"),MB_OK|MB_ICONINFORMATION);
		return 0;	//nothing to do
	}

	//ファイル名を取得
	CString strIniName;
	if(PATHERROR_NONE!=UtilGetCompletePathName(strIniName,cmdParams[1]))return 1;		//処理対象が見つからない

	//---INI解析

	LPCTSTR extArray[]={	//関連づけ対象となる拡張子の一覧;セクション解析が面倒なので。
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

	//関連づけの処理
	bool bChanged=false;	//設定が変更されればtrue
	CString strCmd;	//関連づけコマンド
	{
		CPath tmp(UtilGetModuleDirectoryPath());	//本体のパス取得
		tmp+=_T("LhaForge.exe");
		tmp.QuoteSpaces();
		strCmd=(CString)tmp;
		strCmd+=_T(" /m \"%1\"");	//パラメータ
	}

	for(UINT i=0;i<extCount;i++){
		//セクションが存在するかどうか確認
		TRACE(_T("Processing Assoc:%s\n"),extArray[i]);
		if(UtilCheckINISectionExists(extArray[i],strIniName)){
			TRACE(_T("Section found\n"));
			int nAction=GetPrivateProfileInt(extArray[i],_T("set"),-1,strIniName);
			if(0==nAction){	//関連づけ削除
				AssocDeleteAssociation(extArray[i]);
			}else if(1==nAction){	//新規に関連づけ作成
				//古い関連づけ設定を取得
				ASSOCINFO ai;
				ai.Ext=extArray[i];
				AssocGetAssociation(ai);

				//関連づけコマンド
				//GetPrivateProfileString(extArray[i],_T("cmd"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
				ai.ShellOpenCommand=strCmd;

				//アイコン情報
				TCHAR szBuffer[_MAX_PATH*2]={0};
				GetPrivateProfileString(extArray[i],_T("iconfile"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
				ai.IconFile=szBuffer;
				ai.IconIndex=GetPrivateProfileInt(extArray[i],_T("iconindex"),0,strIniName);

				//設定実行
				AssocSetAssociation(ai);

				bChanged=true;
			}
		}
	}
	if(bChanged){
		::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSH,NULL,NULL);	//関連付けの変更をシェルに通知
	}

	//シェル拡張の処理
	if(UtilCheckINISectionExists(_T("Shell"),strIniName)){
		//DLLのパス
		CPath tmp(UtilGetModuleDirectoryPath());
#ifdef WIN64
		tmp+=_T("ShellExtDLL64.dll");
#else
		tmp+=_T("ShellExtDLL.dll");
#endif

		//処理内容
		int nAction=GetPrivateProfileInt(_T("Shell"),_T("set"),-1,strIniName);

		if(0==nAction){
			//解除
			ShellUnregisterServer(NULL,tmp);
		}else if(1==nAction){
			//セット
			ShellRegisterServer(NULL,tmp);
		}

		//変更をシステムに通知
		//::SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0L,NULL,SMTO_NORMAL,1000,NULL);
	}

	//変更をシステムに通知(乱暴に)
//	::SHChangeNotify(SHCNE_ALLEVENTS,SHCNF_FLUSH,NULL,NULL);

	{
		//削除するかどうか;誤操作による意図しない削除を防ぐため、特定のキーフレーズが含まれる場合のみ削除を行うようにする
		TCHAR szBuffer[_MAX_PATH*2]={0};
		GetPrivateProfileString(_T("PostProcess"),_T("DeleteMe"),_T(""),szBuffer,sizeof(szBuffer)/sizeof(szBuffer[0])-1,strIniName);
		const CString strKeyPhrase(_T("Please_Delete_Me"));
		if(szBuffer==strKeyPhrase){
			::DeleteFile(strIniName);
		}
	}

	return 0;
}

