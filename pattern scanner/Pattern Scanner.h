#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

#include "Memory Pattern Scanner.h"

//Clears Console Screen (Service func)
void cls();

//Returns ProcessID of Application
DWORD GetPID(TCHAR* ProcessName);

//Finds an address of module and size of module in the process, if fails returns FALSE
BOOL GetModuleBaseAddress(TCHAR* moduleName, DWORD PID, PMODULEENTRY32 addressOutput);

//Scans Region of a Memory for a Pattern
BYTE* PatternScan(Memory_Pattern_Scanner target, BYTE* startAddress, intptr_t addrRegSize);

//Searches for available for scan memory regions, and uses PatternScan() on them
BYTE* ExtScan(Memory_Pattern_Scanner target, BYTE* startAddress, intptr_t size, HANDLE hProcess);