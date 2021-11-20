#pragma once

class Config {
public:
	bool EnableUnlock_FPS = false;
	bool EnableDouble_Attack = false;
	bool EnableRapid_Fire = false;
	bool EnableChest_ESP = false;
	bool EnableLevel_ESP = false;
	bool EnableHealth_Bar = false;
	bool EnableWaterMark = false;
	bool Enable_FlyHack = false;

	bool EnableNo_CoolDown = false;
	bool EnableNo_QCharge = false;
	bool EnableInfStamina = false;
	bool Enable_Freeze_Enemy = false;
	bool Enable_Freeze_BowCharge = false;
	bool Enable_LockY = false;

	bool arrayList = false;
	bool position = false;
	float Saturation = 100;
	float Brightness = 100;

	float rainbowSpeed = 2.f;
	float x, z = 0;

	__int32 Unlock_FPS = 60;
};