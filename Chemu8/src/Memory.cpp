#include "Memory.h"
#include <iostream>
#include <string>
#include <format>
#include <fstream>
#include <vector>

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

bool Memory::LoadROM(const char* filepath)
{
	if (!filepath) return false;
	std::ifstream in(filepath, std::ios::binary | std::ios::ate);
	if (!in) return false;
	std::streamsize size = in.tellg();
	in.seekg(0, std::ios::beg);
	if (size <= 0) return false;

	const unsigned short loadAddr = 0x200;
	const int maxSize = sizeof(m_Memory) - loadAddr;
	if (size > maxSize) return false; // ROM too large to fit

	std::vector<char> buffer((size_t)size);
	if (!in.read(buffer.data(), size)) return false;

	for (std::streamsize i = 0; i < size; ++i)
		m_Memory[loadAddr + (size_t)i] = static_cast<unsigned char>(buffer[(size_t)i]);

	return true;
}

unsigned char Memory::FetchData(unsigned short addr)
{
	return m_Memory[addr];
}

void Memory::WriteData(unsigned short addr, unsigned char value)
{
	m_Memory[addr] = value;
}

bool Memory::PushStack(unsigned short addr)
{
	// ensure sp is within bounds 0..15
	if (sp >= m_StackSize) return false;
	m_Stack[sp] = addr;
	sp++;
	return true;
}

bool Memory::PopStack(unsigned short &outAddr)
{
	if (sp == 0) return false;
	sp--;
	outAddr = m_Stack[sp];
	return true;
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