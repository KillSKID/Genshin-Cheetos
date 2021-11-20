#include <pch.h>
#include "Pattern Scanner.h"

void cls()
{
	system("cls");
}

DWORD GetPID(TCHAR* ProcessName)
{
	PROCESSENTRY32 pe32{};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnapshot == INVALID_HANDLE_VALUE || hProcessSnapshot == NULL)
		return NULL;

	DWORD PID = NULL;

	if (Process32First(hProcessSnapshot, &pe32))
	{
		do
		{
			if (_tcscmp(pe32.szExeFile, ProcessName) == 0)
			{
				PID = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hProcessSnapshot, &pe32));
	}

	CloseHandle(hProcessSnapshot);
	return PID;
}

BOOL GetModuleBaseAddress(TCHAR* moduleName, DWORD PID, PMODULEENTRY32 moduleOutput) {
	MODULEENTRY32 me32{};
	me32.dwSize = sizeof(MODULEENTRY32);

	HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hModuleSnapshot == INVALID_HANDLE_VALUE || hModuleSnapshot == NULL)
		return FALSE;

	if (Module32First(hModuleSnapshot, &me32)) {
		do {
			if (_tcscmp(me32.szModule, moduleName) == 0) {
				moduleOutput->modBaseAddr = me32.modBaseAddr;
				moduleOutput->modBaseSize = me32.modBaseSize;
				return TRUE;
			}
		} while (Module32Next(hModuleSnapshot, &me32));
	}

	CloseHandle(hModuleSnapshot);
	return FALSE;
}

BYTE* PatternScan(Memory_Pattern_Scanner target, BYTE* startAddress, intptr_t addrRegSize)
{
	for (intptr_t i = 0; i < addrRegSize; ++i)
	{
		bool isFound = true;

		for (intptr_t j = 0; j < target.m_Length; ++j)
		{
			if (target.m_Mask[j] != '?' && target.m_Pattern[j] != *(BYTE*)((intptr_t)startAddress + i + j))
			{
				isFound = false;
				break;
			}
		}

		if (isFound)
		{
			return startAddress + i;
		}
	}

	return nullptr;
}

BYTE* ExtScan(Memory_Pattern_Scanner target, BYTE* startAddress, intptr_t size, HANDLE hProcess)
{
	BYTE* match(nullptr);	//Contains an address of pattern in process memory
	BYTE* buffer(nullptr);	//Contains pointer to scan region

	MEMORY_BASIC_INFORMATION mbi;
	mbi.RegionSize = 0x1000;

	SIZE_T bytesRead;	//Number of Bytes read while searching for pattern
	DWORD oldProtect;	//Contains old protect attributes

	VirtualQueryEx(hProcess, (LPCVOID)startAddress, &mbi, sizeof(mbi));

	for (BYTE* current = startAddress; current < startAddress + size; current += mbi.RegionSize)
	{
		if (!VirtualQueryEx(hProcess, current, &mbi, sizeof(mbi)))
			continue;
		if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
			continue;

		delete[] buffer;
		buffer = new BYTE[mbi.RegionSize];

		if (VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);
			VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);

			BYTE* internalAddress = PatternScan(target, buffer, (intptr_t)bytesRead);

			if (internalAddress != NULL)
			{
				//Converting address from internal to external
				match = current + (internalAddress - buffer);
				break;
			}
		}
	}

	delete[] buffer;
	return match;
}