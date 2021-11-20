#pragma once

typedef unsigned char BYTE;

// FPS unlocker
BYTE fpsFuncPattern[]{ 0x7F, 0x0F, 0x8B, 0x05, 0xFF , 0xFF , 0xFF , 0xFF };
BYTE fpsFuncMask[]{ 'x', 'x', 'x', 'x', '?' , '?' , '?' , '?' };
