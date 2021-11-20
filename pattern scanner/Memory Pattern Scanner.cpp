#include <pch.h>
#include "Memory Pattern Scanner.h"

Memory_Pattern_Scanner::Memory_Pattern_Scanner(const Memory_Pattern_Scanner& toCopy)
{
	m_Length = toCopy.m_Length;
	m_Pattern = new BYTE[m_Length];
	m_Mask = new BYTE[m_Length];

	for (size_t i = 0; i < m_Length; ++i)
	{
		m_Pattern[i] = toCopy.m_Pattern[i];
		m_Mask[i] = toCopy.m_Mask[i];
	}
}

Memory_Pattern_Scanner::Memory_Pattern_Scanner(const BYTE* pattern, const BYTE* mask, const size_t length) : m_Length(length)
{
	m_Pattern = new BYTE[length];
	m_Mask = new BYTE[length];

	for (size_t i = 0; i < length; ++i)
	{
		m_Pattern[i] = pattern[i];
		m_Mask[i] = mask[i];
	}
}

Memory_Pattern_Scanner::~Memory_Pattern_Scanner()
{
	delete[] m_Pattern;
	delete[] m_Mask;
	m_Pattern = m_Mask = nullptr;
}