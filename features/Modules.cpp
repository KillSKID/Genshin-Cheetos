#include <pch.h>
#include <thread>
#include <cmath>

#include "../pattern scanner/Memory Pattern Scanner.h"
#include "../pattern scanner/Patterns List.h"
#include "../pattern scanner/Pattern Scanner.h"
#include "../pattern scanner/Patcher.h"
#include "../configs/Configs.h"

Config cfg;
extern int AnimateAlpha;
extern float h;
extern bool ShowAnimation;

float animationDelta = 0.0f;

int Run(DWORD processID)
{
	static float animationDelta = 0;
	Memory_Pattern_Scanner Pattern_FPS(fpsFuncPattern, fpsFuncMask, sizeof(fpsFuncPattern) / sizeof(BYTE));

	MODULEENTRY32 UnityPlayer{};
	MODULEENTRY32 UserAssembly{};
	MODULEENTRY32 em = {};

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

	std::wstring unityPlayer(L"UnityPlayer.dll");
	const wchar_t* unityPlayerName = unityPlayer.c_str();

	std::wstring unityAssembly(L"UserAssembly.dll");
	const wchar_t* unityAssemblyName = unityAssembly.c_str();

	UnityPlayer = GetModule(processID, unityPlayerName);

	printf("UnityPlayer: %X%X\n", (uintptr_t)UnityPlayer.modBaseAddr >> 32 & -1, UnityPlayer.modBaseAddr);

	UserAssembly = GetModule(processID, unityAssemblyName);

	printf("UserAssembly: %X%X\n", (uintptr_t)UserAssembly.modBaseAddr >> 32 & -1, UserAssembly.modBaseAddr);

	DWORD64 dwFpsFuncAddress = (DWORD64)ExtScan(Pattern_FPS, UnityPlayer.modBaseAddr, (uintptr_t)UnityPlayer.modBaseSize, hProcess);
	DWORD64 dwLimiterAddress = NULL;

	if (!ReadProcessMemory(hProcess, (LPCVOID)(dwFpsFuncAddress + 4), &dwLimiterAddress, sizeof(__int32), 0))
		std::cout << GetLastError() << std::endl;

	dwLimiterAddress += dwFpsFuncAddress + 0x8;

	DWORD dwExitCode = STILL_ACTIVE;

	while (dwExitCode == STILL_ACTIVE)
	{
		__int32 currentFPS = NULL;
		__int32 targetFPS = cfg.Unlock_FPS;

		GetExitCodeProcess(hProcess, &dwExitCode);

		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100 ticks looping

		h += cfg.rainbowSpeed;
		if (h > 360) {
			h = 0;
		}

		if (ShowAnimation && animationDelta < 255) {
			animationDelta += 5;
		}
		else if (animationDelta > 0) {
			animationDelta -= 5;
		}

		float animationF = (animationDelta / 255.f);

		AnimateAlpha = 255.f * (animationF < 0.5 ? 8 * animationF * animationF * animationF * animationF : 1 - pow(-2 * animationF + 2, 4) / 2);

		if (cfg.EnableUnlock_FPS) {
			ReadProcessMemory(hProcess, (LPCVOID)dwLimiterAddress, &currentFPS, sizeof(__int32), 0);
			if (currentFPS == -1)
				return 0;
			if (currentFPS != targetFPS) {
				WriteProcessMemory(hProcess, (LPVOID)dwLimiterAddress, &targetFPS, sizeof(__int32), 0);
			}
		}

		if (cfg.EnableLevel_ESP) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x3556638), (BYTE*)"\x84", 1);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x3556638), (BYTE*)"\x87", 1);
		}

		if (cfg.EnableInfStamina) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x74A45DD), (BYTE*)"\x10", 1);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x74A45DD), (BYTE*)"\x11", 1);
		}

		if (cfg.EnableNo_CoolDown) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x29CCC46), (BYTE*)"\x10", 1);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x29CCC46), (BYTE*)"\x11", 1);
		}

		if (cfg.EnableNo_QCharge) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x29B37E8), (BYTE*)"\x0F\x11\x70\x30", 4);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x29B37E8), (BYTE*)"\x0F\x10\x70\x30", 4);
		}

		if (cfg.Enable_Freeze_BowCharge) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x69D1040), (BYTE*)"\x10", 1);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x69D1040), (BYTE*)"\x11", 1);
		}

		if (cfg.Enable_Freeze_Enemy) {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x1783914), (BYTE*)"\x0F\x85\xA1\x00\x00\x00", 6);
			Nop((BYTE*)(UserAssembly.modBaseAddr + 0x258AF50), 2);
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x32E1080), (BYTE*)"\xE8\x1B\x75\xAB\x04", 5);
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x4225342), (BYTE*)"\x0F\x85\xF6\x07\x00\x00", 6);
		}
		else {
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x1783914), (BYTE*)"\x0F\x84\xA1\x00\x00\x00", 6);
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x258AF50), (BYTE*)"\xB0\x01", 2);
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x32E1080), (BYTE*)"\xE8\x1B\x75\xAB\x04", 5);
			Patch((BYTE*)(UserAssembly.modBaseAddr + 0x4225342), (BYTE*)"\x0F\x84\xF6\x07\x00\x00", 6);
		}

		if (cfg.EnableDouble_Attack) {
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0xC61A28), (BYTE*)"\x10", 1);
		}
		else {
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0xC61A28), (BYTE*)"\x8B\x47\x30\x89\x47\x2C", 6);
		}

		if (cfg.Enable_FlyHack) {
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x1123F44), (BYTE*)"\x10", 1);
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x11333AB), (BYTE*)"\x10", 1);
		}
		else {
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x1123F44), (BYTE*)"\x89", 1);
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x11333AB), (BYTE*)"\x89", 1);
		}

		if (cfg.Enable_LockY) {
			//x14 down x15 lock
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x119FBE0), (BYTE*)"\x15", 1);
		}
		else {
			Patch((BYTE*)(UnityPlayer.modBaseAddr + 0x119FBE0), (BYTE*)"\xF3", 1);
		}

		cfg.x = *(float*)(uintptr_t*)(UnityPlayer.modBaseAddr + 0x196A2F8);
		cfg.z = *(float*)(uintptr_t*)(UnityPlayer.modBaseAddr + 0x196A2F4);
	}
}