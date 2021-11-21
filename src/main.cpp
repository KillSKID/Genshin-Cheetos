#include "pch.h"
#include "base.h"

DWORD FindGenshin()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	Process32First(hSnapshot, &pe);

	DWORD pid = 0;
	while (Process32Next(hSnapshot, &pe))
	{
		if (wcscmp(pe.szExeFile, TEXT("GenshinImpact.exe")) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
	}

	CloseHandle(hSnapshot);
	return pid;
}

MODULEENTRY32 GetModule(DWORD procId, const wchar_t* str)
{
	MODULEENTRY32 mod = {};
	HANDLE hSnap;
	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId)) == NULL)
		return mod;

	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(modEntry);
	if (Module32First(hSnap, &modEntry))
	{
		do
		{
			if (lstrcmpW(modEntry.szModule, str) == 0)
			{
				mod = modEntry;
				break;
			}
		} while (Module32Next(hSnap, &modEntry));
	}

	CloseHandle(hSnap);
	return mod;
}

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	Base::Data::hModule = (HMODULE)lpThreadParameter;
	Base::Init();

	if (!AllocConsole())
	{
		return FALSE;
	}

	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	SetConsoleTitle(TEXT("GenshinImpact"));

	printf("[+] base address: 0x%llX\n", GetModuleHandle(0));

	Run(FindGenshin());

	FreeConsole();

	return TRUE;
}

DWORD WINAPI ExitThread(LPVOID lpThreadParameter)
{
	if (!Base::Data::Detached)
	{
		Base::Data::Detached = true;
		FreeLibraryAndExitThread(Base::Data::hModule, TRUE);
	}
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		if (!Base::Data::Detached)
			CreateThread(nullptr, 0, ExitThread, hModule, 0, nullptr);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}