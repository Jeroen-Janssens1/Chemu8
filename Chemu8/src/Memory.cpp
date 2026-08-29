#include "Memory.h"
#include <iostream>
#include <string>
#include <format>

Memory::Memory()
	:memory{},
	stack{}
{
	// load in the font into the memory to 'initialize' it
	int fontsetSize = sizeof(chip8_fontset);
	for (int i = 0; i < fontsetSize; ++i)
		memory[i+fontMemOffset] = chip8_fontset[i]; // store fontset into memory from 0x050 to 0x09F
}

unsigned char Memory::FetchData(unsigned short addr)
{
	return memory[addr];
}

#ifdef _DEBUG
void Memory::PrintRAM(unsigned short startAddr, unsigned short endAddr)
{
	int memorySize = sizeof(memory);
	std::cout << "RAM Dump:" << std::endl;
	for (int i = 0; i < memorySize; i++)
		std::cout << std::format("{:x}", i) << ": " << std::to_string(memory[i]) << std::endl;
}
#endif