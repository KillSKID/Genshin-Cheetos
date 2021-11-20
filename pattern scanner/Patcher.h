#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

void Patch(BYTE* dst, BYTE* src, unsigned int size);
void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
void Nop(BYTE* dst, unsigned int size);
void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);