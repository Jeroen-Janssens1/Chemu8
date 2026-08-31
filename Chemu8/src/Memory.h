#pragma once
class Memory final
{
public:
	Memory();
	unsigned char FetchData(unsigned short addr);
	void WriteData(unsigned short addr, unsigned char value);
	// Load a ROM file into memory at the program start address (0x200).
	bool LoadROM(const char* filepath);
	// Push a return address onto the stack. Returns true on success, false if stack is full.
	bool PushStack(unsigned short addr);
	// Pop a return address from the stack. Returns true on success, false if stack is empty.
	bool PopStack(unsigned short &outAddr);

	#ifdef _DEBUG
	void PrintRAM(unsigned short startAddr, unsigned short endAddr); // debug function to print RAM memory to console window to check RAM state
	#endif

private:
	unsigned short m_FontMemOffset = 0x050;
	unsigned char m_Memory[4096]; // our RAM memory
	unsigned short m_Stack[16]; // we store the stack as an array of shorts because this will only hold memory addresses (16 bit values).
	const unsigned char m_StackSize = 16; // the size of the stack (16 addresses)
	unsigned short sp; // the stack pointer
	const unsigned char m_Chip8Fontset[80] = // the fontset which will be loaded into memory later. Stored as var here for easy editing later
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

