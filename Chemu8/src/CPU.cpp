#include "CPU.h"
#include "Memory.h"
#include "Graphics.h"
#include "Audio.h"
#include "InputHandler.h"
#include <cstdlib>
#include <iostream>

// Call stack is stored in Memory; CPU will use Memory::PushStack/PopStack to
// manage CALL/RET.

CPU::CPU(Memory* memory, Graphics* graphics, InputHandler* input, Audio* audio)
	: m_pMemory(memory), m_pGraphics(graphics), m_pInput(input), m_pAudio(audio)
{
	// Program counter starts at 0x200 in most CHIP-8 implementations
	m_Pc = 0x200;
	m_Opcode = 0;
	// nothing to initialize here for stack (managed by Memory)

	// Initialize primary table to NULL handlers
	for (int i = 0; i < 16; ++i) m_OpTable[i] = &CPU::OpcodeNULL;
	m_OpTable[0x0] = &CPU::Opcode0;
	m_OpTable[0x1] = &CPU::Opcode1;
	m_OpTable[0x2] = &CPU::Opcode2;
	m_OpTable[0x3] = &CPU::Opcode3;
	m_OpTable[0x4] = &CPU::Opcode4;
	m_OpTable[0x5] = &CPU::Opcode5;
	m_OpTable[0x6] = &CPU::Opcode6;
	m_OpTable[0x7] = &CPU::Opcode7;
	m_OpTable[0x8] = &CPU::Opcode8;
	m_OpTable[0x9] = &CPU::Opcode9;
	m_OpTable[0xA] = &CPU::OpcodeA;
	m_OpTable[0xB] = &CPU::OpcodeB;
	m_OpTable[0xC] = &CPU::OpcodeC;
	m_OpTable[0xD] = &CPU::OpcodeD;
	m_OpTable[0xE] = &CPU::OpcodeE;
	m_OpTable[0xF] = &CPU::OpcodeF;

	// 0x0 sub-table: default to NULL handler
	for (int i = 0; i < 16; ++i) m_Table0[i] = &CPU::OpcodeNULL;
	m_Table0[0x0] = &CPU::Opcode00E0;
	m_Table0[0xE] = &CPU::Opcode00EE;

	// 0x8 sub-table (last nibble determines operation)
	for (int i = 0; i < 16; ++i) m_Table8[i] = &CPU::OpcodeNULL;
	m_Table8[0x0] = &CPU::Opcode8xy0;
	m_Table8[0x1] = &CPU::Opcode8xy1;
	m_Table8[0x2] = &CPU::Opcode8xy2;
	m_Table8[0x3] = &CPU::Opcode8xy3;
	m_Table8[0x4] = &CPU::Opcode8xy4;
	m_Table8[0x5] = &CPU::Opcode8xy5;
	m_Table8[0x6] = &CPU::Opcode8xy6;
	m_Table8[0x7] = &CPU::Opcode8xy7;
	m_Table8[0xE] = &CPU::Opcode8xyE;

	// 0xF sub-table: default to NULL
	for (int i = 0; i < 256; ++i) m_TableF[i] = &CPU::OpcodeNULL;
	m_TableF[0x07] = &CPU::OpcodeFx07;
	m_TableF[0x0A] = &CPU::OpcodeFx0A;
	m_TableF[0x15] = &CPU::OpcodeFx15;
	m_TableF[0x18] = &CPU::OpcodeFx18;
	m_TableF[0x1E] = &CPU::OpcodeFx1E;
	m_TableF[0x29] = &CPU::OpcodeFx29;
	m_TableF[0x33] = &CPU::OpcodeFx33;
	m_TableF[0x55] = &CPU::OpcodeFx55;
	m_TableF[0x65] = &CPU::OpcodeFx65;

	// Audio is provided by the caller (main); assign pointer (non-owning)
	// m_pAudio stays as passed in constructor initializer list
}

void CPU::RunCycle()
{
	// fetch opcode
	m_Opcode = (m_pMemory->FetchData(m_Pc) << 8u) | m_pMemory->FetchData(m_Pc + 1);
	// execute current opcode (m_Opcode should be set by the fetch stage)
	unsigned short firstNibble = (m_Opcode & 0xF000u) >> 12u;
	auto handler = m_OpTable[firstNibble];
	if (handler)
		(this->*handler)();
}

void CPU::UpdateTimers(float deltaSeconds)
{
	if (deltaSeconds <= 0.0f) return;
	m_TimerAcc += deltaSeconds;
	while (m_TimerAcc >= m_Interval)
	{
		if (m_DelayTimer > 0) --m_DelayTimer;
		if (m_SoundTimer > 0) --m_SoundTimer;
		m_TimerAcc -= m_Interval;
	}
	// Control audio playback based on sound timer
	if (m_pAudio)
	{
		if (m_SoundTimer > 0) m_pAudio->SetPlaying(true);
		else m_pAudio->SetPlaying(false);
	}
}

// Primary group handlers
void CPU::Opcode0()
{
	unsigned short low = m_Opcode & 0x000Fu;
	auto handler = m_Table0[low];
	if (handler) (this->*handler)();
}

void CPU::Opcode1() { Opcode1nnn(); }
void CPU::Opcode2() { Opcode2nnn(); }
void CPU::Opcode3() { Opcode3xkk(); }
void CPU::Opcode4() { Opcode4xkk(); }
void CPU::Opcode5() { Opcode5xy0(); }
void CPU::Opcode6() { Opcode6xkk(); }
void CPU::Opcode7() { Opcode7xkk(); }
void CPU::Opcode8()
{
	unsigned short low = m_Opcode & 0x000Fu;
	auto handler = m_Table8[low];
	if (handler) (this->*handler)();
}
void CPU::Opcode9() { Opcode9xy0(); }
void CPU::OpcodeA() { OpcodeAnnn(); }
void CPU::OpcodeB() { OpcodeBnnn(); }
void CPU::OpcodeC() { OpcodeCxkk(); }
void CPU::OpcodeD() { OpcodeDxyn(); }
void CPU::OpcodeE()
{
	unsigned short byte = m_Opcode & 0x00FFu;
	if (byte == 0x9E) OpcodeEx9E();
	else if (byte == 0xA1) OpcodeExA1();
	else OpcodeNULL();
}
void CPU::OpcodeF()
{
	unsigned short byte = m_Opcode & 0x00FFu;
	auto handler = m_TableF[byte];
	if (handler) (this->*handler)();
}

// NULL handler
void CPU::OpcodeNULL()
{
	// Unimplemented or invalid opcode: advance PC to avoid stalling
	std::cerr << "OpcodeNULL encountered: 0x" << std::hex << m_Opcode << "\n";
	m_Pc += 2;
}

// 0x0 group implementations
void CPU::Opcode00E0()
{
	// Clear screen - Graphics interface not fully specified here; assume graphics will be handled elsewhere
	m_Pc += 2;
}

void CPU::Opcode00EE()
{
	if (m_pMemory)
	{
		unsigned short retAddr = 0;
		if (m_pMemory->PopStack(retAddr))
		{
			m_Pc = retAddr;
			return;
		}
	}
	// stack underflow or no memory: treat as NOP
	m_Pc += 2;
}

// 0x8 group implementations (arithmetic/logic)
void CPU::Opcode8xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[x] = m_V[y];
	m_Pc += 2;
}

void CPU::Opcode8xy1()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[x] |= m_V[y];
	m_Pc += 2;
}

void CPU::Opcode8xy2()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[x] &= m_V[y];
	m_Pc += 2;
}

void CPU::Opcode8xy3()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[x] ^= m_V[y];
	m_Pc += 2;
}

void CPU::Opcode8xy4()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	unsigned short sum = (unsigned short)m_V[x] + (unsigned short)m_V[y];
	m_V[0xF] = (sum > 0xFF) ? 1 : 0;
	m_V[x] = (unsigned char)(sum & 0xFF);
	m_Pc += 2;
}

void CPU::Opcode8xy5()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[0xF] = (m_V[x] > m_V[y]) ? 1 : 0;
	m_V[x] = m_V[x] - m_V[y];
	m_Pc += 2;
}

void CPU::Opcode8xy6()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Shift right: VF set to least significant bit prior to shift
	m_V[0xF] = m_V[x] & 0x1;
	m_V[x] >>= 1;
	m_Pc += 2;
}

void CPU::Opcode8xy7()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[0xF] = (m_V[y] > m_V[x]) ? 1 : 0;
	m_V[x] = m_V[y] - m_V[x];
	m_Pc += 2;
}

void CPU::Opcode8xyE()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Shift left: VF set to most significant bit prior to shift
	m_V[0xF] = (m_V[x] & 0x80) >> 7;
	m_V[x] <<= 1;
	m_Pc += 2;
}

// 0xE group (input related)
void CPU::OpcodeEx9E()
{
	// Skip next instruction if key with the value of Vx is pressed
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char key = m_V[x] & 0x0F;
	if (m_pInput)
	{
		if (m_pInput->GetKeyState(key)) m_Pc += 4; else m_Pc += 2;
	}
	else
	{
		// no input handler: behave as no key pressed
		m_Pc += 2;
	}
}

void CPU::OpcodeExA1()
{
	// Skip next instruction if key with the value of Vx is not pressed
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char key = m_V[x] & 0x0F;
	if (m_pInput)
	{
		if (!m_pInput->GetKeyState(key)) m_Pc += 4; else m_Pc += 2;
	}
	else
	{
		// no input handler: behave as key not pressed
		m_Pc += 4;
	}
}

// 0xF group
void CPU::OpcodeFx07()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_V[x] = m_DelayTimer;
	m_Pc += 2;
}

void CPU::OpcodeFx0A()
{
	// Wait for a key press, store the value of the key in Vx
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	if (m_pInput)
	{
		unsigned char pressedKey = 0;
		if (m_pInput->AnyKeyPressed(pressedKey))
		{
			m_V[x] = pressedKey;
			m_Pc += 2;
		}
		else
		{
			// do not advance PC; blocking behaviour until key pressed
		}
	}
	else
	{
		// no input handler: skip
		m_Pc += 2;
	}
}

void CPU::OpcodeFx15()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_DelayTimer = m_V[x];
	m_Pc += 2;
}

void CPU::OpcodeFx18()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_SoundTimer = m_V[x];
	m_Pc += 2;
}

void CPU::OpcodeFx1E()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_I = m_I + m_V[x];
	m_Pc += 2;
}

void CPU::OpcodeFx29()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Each font character is 5 bytes and stored at 0x050
	m_I = 0x050 + (m_V[x] * 5);
	m_Pc += 2;
}

void CPU::OpcodeFx33()
{
	if (!m_pMemory) { OpcodeNULL(); return; }
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char value = m_V[x];
	unsigned char hundreds = value / 100;
	unsigned char tens = (value / 10) % 10;
	unsigned char ones = value % 10;
	m_pMemory->WriteData(m_I, hundreds);
	m_pMemory->WriteData(m_I + 1, tens);
	m_pMemory->WriteData(m_I + 2, ones);
	m_Pc += 2;
}

void CPU::OpcodeFx55()
{
	if (!m_pMemory) { OpcodeNULL(); return; }
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	for (unsigned char idx = 0; idx <= x; ++idx)
		m_pMemory->WriteData(m_I + idx, m_V[idx]);
	m_Pc += 2;
}

void CPU::OpcodeFx65()
{
	if (!m_pMemory) { OpcodeNULL(); return; }
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	for (unsigned char idx = 0; idx <= x; ++idx)
		m_V[idx] = m_pMemory->FetchData(m_I + idx);
	m_Pc += 2;
}

// Other specific opcodes
void CPU::Opcode1nnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_Pc = addr;
}

void CPU::Opcode2nnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	if (m_pMemory)
	{
		if (m_pMemory->PushStack((unsigned short)(m_Pc + 2)))
		{
			m_Pc = addr;
			return;
		}
	}
	// stack overflow or no memory: treat as error
	OpcodeNULL();
}

void CPU::Opcode3xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	if (m_V[x] == kk) m_Pc += 4; else m_Pc += 2;
}

void CPU::Opcode4xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	if (m_V[x] != kk) m_Pc += 4; else m_Pc += 2;
}

void CPU::Opcode5xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	if (m_V[x] == m_V[y]) m_Pc += 4; else m_Pc += 2;
}

void CPU::Opcode6xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = kk;
	m_Pc += 2;
}

void CPU::Opcode7xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = m_V[x] + kk;
	m_Pc += 2;
}

void CPU::Opcode9xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	if (m_V[x] != m_V[y]) m_Pc += 4; else m_Pc += 2;
}

void CPU::OpcodeAnnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_I = addr;
	m_Pc += 2;
}

void CPU::OpcodeBnnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_Pc = (unsigned short)(m_V[0] + addr);
}

void CPU::OpcodeCxkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = (unsigned char)(std::rand() & kk);
	m_Pc += 2;
}

void CPU::OpcodeDxyn()
{
	// Draw sprite at coordinate (Vx, Vy) with N bytes of sprite data starting at I
	if (!m_pGraphics || !m_pMemory) { OpcodeNULL(); return; } // graphics or memory not available
	unsigned char x = m_V[(m_Opcode & 0x0F00u) >> 8u];
	unsigned char y = m_V[(m_Opcode & 0x00F0u) >> 4u];
	unsigned char height = m_Opcode & 0x000Fu;
	// Since Graphics::SetPixel does not report collision, set VF to 0
	m_V[0xF] = 0;
	for (unsigned char row = 0; row < height; ++row)
	{
		unsigned char spriteByte = m_pMemory->FetchData(m_I + row);
		for (unsigned char bit = 0; bit < 8; ++bit)
		{
			if ((spriteByte & (0x80 >> bit)) != 0)
			{
				unsigned char px = (x + bit) % 64; // wrap horizontally (64)
				unsigned char py = (y + row) % 32; // wrap vertically (32)
				m_pGraphics->SetPixel(px, py);
			}
		}
	}
	m_Pc += 2;
}
