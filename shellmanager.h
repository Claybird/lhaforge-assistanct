#pragma once

//�V�F���g��DLL���Ǘ�����

bool ShellRegisterServer(HWND,LPCTSTR);
bool ShellUnregisterServer(HWND,LPCTSTR);

BOOL SetKeyAndValue(HKEY inKeyRootH,LPCTSTR inKey,LPCTSTR inValueName,LPCTSTR inValue);
//�ċN�I���W�X�g���폜
LONG RecursiveDeleteKey(HKEY inKeyParentH,LPCTSTR inKeyChild);
//���O�t���l�̍폜
BOOL DeleteNamedValue(HKEY inKeyRootH,LPCTSTR inSubKey,LPCTSTR inValueName);

