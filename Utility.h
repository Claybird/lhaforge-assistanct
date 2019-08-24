//LhaForge用

//デバッグ用関数および便利な関数群
#pragma once
//#pragma warning(disable:4786)

#if defined(_DEBUG) || defined(DEBUG)

void TraceLastError();

#else
// Releaseのとき
#define TraceLastError

#endif	//_DEBUG

//=============================================
// 共通便利関数
//=============================================

//エラーメッセージを表示
int ErrorMessage(LPCTSTR);
//メッセージキャプションを取得
LPCTSTR UtilGetMessageCaption();
void UtilGetLastErrorMessage(CString &strMsg);

#define BOOL2bool(x)	(FALSE!=x)

//コマンドライン引数を取得(個数を返す)
int UtilGetCommandLineParams(std::vector<CString> &rParams);

//INIに数字を文字列として書き込む
BOOL UtilWritePrivateProfileInt(LPCTSTR lpAppName,LPCTSTR lpKeyName,LONG nData,LPCTSTR lpFileName);

//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName);

//指定されたmapがキーを持っているかどうか
template <typename mapclass,typename keyclass>
bool has_key(const mapclass &theMap,keyclass theKey){
	return theMap.find(theKey)!=theMap.end();
}


enum PATHERROR{
	PATHERROR_NONE,		//成功
	PATHERROR_INVALID,	//パラメータ指定が不正
	PATHERROR_ABSPATH,	//絶対パスの取得に失敗
	PATHERROR_NOTFOUND,	//ファイルもしくはフォルダが見つからない
	PATHERROR_LONGNAME,	//ロングファイル名取得失敗
};
//フルパスかつ絶対パスの取得
PATHERROR UtilGetCompletePathName(CString &_FullPath,LPCTSTR lpszFileName);

//自分のプログラムのおいてあるディレクトリのパス名を返す
LPCTSTR UtilGetModuleDirectoryPath();

//指定された値が配列中にあればそのインデックスを探す;無ければ-1
template <typename arrayclass,typename valueclass>
int index_of(const arrayclass &theArray,valueclass theValue){
	for(unsigned int i=0;i<theArray.size();++i){
		if(theArray[i]==theValue){
			return (signed)i;
		}
	}
	return -1;
}
//コンテナの要素を削除する
template <typename arrayclass,typename valueclass>
void remove_item(arrayclass &theArray,const valueclass &theValue){
	theArray.erase(std::remove(theArray.begin(), theArray.end(), theValue), theArray.end());
}
