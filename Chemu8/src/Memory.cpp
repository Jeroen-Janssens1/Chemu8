#include "Memory.h"
#include <iostream>
#include <string>
#include <format>

Memory::Memory()
	:m_Memory{},
	m_Stack{},
	sp{0}
{
	// load in the font into the memory to 'initialize' it
	int fontsetSize = sizeof(m_Chip8Fontset);
	for (int i = 0; i < fontsetSize; ++i)
		m_Memory[i+m_FontMemOffset] = m_Chip8Fontset[i]; // store fontset into memory from 0x050 to 0x09F
}

unsigned char Memory::FetchData(unsigned short addr)
{
	return m_Memory[addr];
}

void Memory::WriteData(unsigned short addr, unsigned char value)
{
	m_Memory[addr] = value;
}

#ifdef _DEBUG
void Memory::PrintRAM(unsigned short startAddr, unsigned short endAddr)
{
	int memorySize = sizeof(m_Memory);
	std::cout << "RAM Dump:" << std::endl;
	for (int i = startAddr; i < memorySize || i < endAddr; i++)
		std::cout << std::format("{:x}", i) << ": " << std::to_string(m_Memory[i]) << std::endl;
}
#endif