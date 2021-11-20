#pragma once

#include <vcruntime.h>

class Memory_Pattern_Scanner
{
public:
	typedef unsigned char BYTE;
public:
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
	Memory_Pattern_Scanner() = delete;
	Memory_Pattern_Scanner(const Memory_Pattern_Scanner& toCopy);
	Memory_Pattern_Scanner(const BYTE* pattern, const BYTE* mask, const size_t size);
	~Memory_Pattern_Scanner();
	friend BYTE* PatternScan(Memory_Pattern_Scanner pattern, BYTE* startAddress, intptr_t size);
private:
	BYTE* m_Pattern;
	BYTE* m_Mask;
	size_t m_Length;
};
