//LhaForge�p

//�f�o�b�O�p�֐�����ѕ֗��Ȋ֐��Q
#pragma once
//#pragma warning(disable:4786)

#if defined(_DEBUG) || defined(DEBUG)

void TraceLastError();

#else
// Release�̂Ƃ�
#define TraceLastError

#endif	//_DEBUG

//=============================================
// ���ʕ֗��֐�
//=============================================

//�G���[���b�Z�[�W��\��
int ErrorMessage(LPCTSTR);
//���b�Z�[�W�L���v�V�������擾
LPCTSTR UtilGetMessageCaption();
void UtilGetLastErrorMessage(CString &strMsg);

#define BOOL2bool(x)	(FALSE!=x)

//�R�}���h���C���������擾(����Ԃ�)
int UtilGetCommandLineParams(std::vector<CString> &rParams);

//INI�ɐ����𕶎���Ƃ��ď�������
BOOL UtilWritePrivateProfileInt(LPCTSTR lpAppName,LPCTSTR lpKeyName,LONG nData,LPCTSTR lpFileName);

//INI�Ɏw�肳�ꂽ�Z�N�V����������Ȃ�true��Ԃ�
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName);

//�w�肳�ꂽmap���L�[�������Ă��邩�ǂ���
template <typename mapclass,typename keyclass>
bool has_key(const mapclass &theMap,keyclass theKey){
	return theMap.find(theKey)!=theMap.end();
}


enum PATHERROR{
	PATHERROR_NONE,		//����
	PATHERROR_INVALID,	//�p�����[�^�w�肪�s��
	PATHERROR_ABSPATH,	//��΃p�X�̎擾�Ɏ��s
	PATHERROR_NOTFOUND,	//�t�@�C���������̓t�H���_��������Ȃ�
	PATHERROR_LONGNAME,	//�����O�t�@�C�����擾���s
};
//�t���p�X����΃p�X�̎擾
PATHERROR UtilGetCompletePathName(CString &_FullPath,LPCTSTR lpszFileName);

//�����̃v���O�����̂����Ă���f�B���N�g���̃p�X����Ԃ�
LPCTSTR UtilGetModuleDirectoryPath();

//�w�肳�ꂽ�l���z�񒆂ɂ���΂��̃C���f�b�N�X��T��;�������-1
template <typename arrayclass,typename valueclass>
int index_of(const arrayclass &theArray,valueclass theValue){
	for(unsigned int i=0;i<theArray.size();++i){
		if(theArray[i]==theValue){
			return (signed)i;
		}
	}
	return -1;
}
//�R���e�i�̗v�f���폜����
template <typename arrayclass,typename valueclass>
void remove_item(arrayclass &theArray,const valueclass &theValue){
	theArray.erase(std::remove(theArray.begin(), theArray.end(), theValue), theArray.end());
}
