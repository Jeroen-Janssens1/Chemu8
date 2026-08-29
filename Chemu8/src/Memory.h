// This massive comment block simply explains the 'documentation' of the CHIP-8 memory (or more percisely the COSMAC VIP RAM memory and how the CHIP-8 interpreter used it).
// For more in-depth information, documentation is available online and linked in the GitHub repository this code is stored on.
// 
// Overview of RAM memory:
// 4096 bytes in size
// first 512 addresses (0x000 till 0x1FF) are reserved to store the interpreter itself (for us these will simply be empty)
// The font needs to be stored somewhere in those first 512 addresses.
//
// Overview of Font:
// 0-9 as well as A,B,C,D,E,F (hexadecimal digits basically) are the existing sprites. Each is 4 pixels wide and 5 pixels tall
// each sprite exists out of 5 bytes of data (total of 16*5 = 80 bytes)
// It seems a loose 'convention' has developed to store the fonts in addresses 050-09F. I will follow this 'convention' here because why not.
//
// Call Stack:
// The Call Stack originally had space for 16 memory addresses (so 32 bytes in size). I will (for now) follow this limitation to stay fateful to the original.

#pragma once

class Memory final
{
public:
	Memory();
	~Memory() = default;

	unsigned char FetchData(unsigned short addr);

	#ifdef _DEBUG
	void PrintRAM(unsigned short startAddr, unsigned short endAddr); // debug function to print RAM memory to console window to check RAM state
	#endif

private:
	unsigned short fontMemOffset = 0x050;
	unsigned char memory[4096]; // our RAM memory
	unsigned short stack[16]; // we store the stack as an array of shorts because this will only hold memory addresses (16 bit values).
	const unsigned char chip8_fontset[80] = // the fontset which will be loaded into memory later. Stored as var here for easy editing later
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
};

