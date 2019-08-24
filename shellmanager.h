#pragma once

//シェル拡張DLLを管理する

bool ShellRegisterServer(HWND,LPCTSTR);
bool ShellUnregisterServer(HWND,LPCTSTR);

BOOL SetKeyAndValue(HKEY inKeyRootH,LPCTSTR inKey,LPCTSTR inValueName,LPCTSTR inValue);
//再起的レジストリ削除
LONG RecursiveDeleteKey(HKEY inKeyParentH,LPCTSTR inKeyChild);
//名前付き値の削除
BOOL DeleteNamedValue(HKEY inKeyRootH,LPCTSTR inSubKey,LPCTSTR inValueName);

