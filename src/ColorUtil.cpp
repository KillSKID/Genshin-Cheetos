#include <pch.h>
#include "ColorUtil.h";

ImVec4 HSVtoRGB(float H, float S, float V) {
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
		H = 360;
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1.f - fabs(fmod(H / 60.0, 2.f) - 1.f));
	float m = v - C;
	float r, g, b;

	if (H >= 0.f && H < 60.f) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60.f && H < 120.f) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120.f && H < 180.f) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180.f && H < 240.f) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240.f && H < 300.f) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	float R = (r + m);
	float G = (g + m);
	float B = (b + m);

	return ImVec4(R, G, B, 1.f);
}

ImColor RGBtoHSV(float red, float green, float blue)
{
	unsigned char rgbMin, rgbMax;

	float h, s, v;

	rgbMin = red < green ? (red < blue ? red : blue) : (green < blue ? green : blue);
	rgbMax = red > green ? (red > blue ? red : blue) : (green > blue ? green : blue);

	v = rgbMax;
	if (v == 0)
	{
		h = 0;
		s = 0;
		return ImColor(h, s, v);
	}

	s = 255 * long(rgbMax - rgbMin) / v;
	if (s == 0)
	{
		h = 0;
		return ImColor(h, s, v);
	}

	if (rgbMax == red)
		h = 0 + 43 * (green - blue) / (rgbMax - rgbMin);
	else if (rgbMax == green)
		h = 85 + 43 * (blue - red) / (rgbMax - rgbMin);
	else
		h = 171 + 43 * (red - green) / (rgbMax - rgbMin);

	return ImColor(h, s, v);
}

ImColor rainbowHSB(__int64 delay, float s, float b) {
	FILETIME f;
	GetSystemTimeAsFileTime(&f);
	(long long)f.dwHighDateTime;
	__int64 nano = ((__int64)f.dwHighDateTime << 32LL) + (__int64)f.dwLowDateTime;
	__int64 currentTimeMillis = (nano - 116444736000000000LL) / 10000;

	double rainbowState = ceil((currentTimeMillis + delay) / 20.0);
	rainbowState = fmod(rainbowState, 360.f);
	return HSVtoRGB(rainbowState, s, b);
}