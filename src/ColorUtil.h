#pragma once
#include <pch.h>

ImVec4 HSVtoRGB(float H, float S, float V);
ImColor RGBtoHSV(float red, float green, float blue);
ImColor rainbowHSB(__int64 delay, float s, float b);