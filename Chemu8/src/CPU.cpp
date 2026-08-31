#include "CPU.h"
#include "Memory.h"
#include "Graphics.h"
#include "Audio.h"
#include "InputHandler.h"
#include <cstdlib>
#include <iostream>

CPU::CPU(Memory* memory, Graphics* graphics, InputHandler* input, Audio* audio)
	: m_pMemory(memory), m_pGraphics(graphics), m_pInput(input), m_pAudio(audio)
{
	// Program counter starts at 0x200 in most CHIP-8 implementations
	m_Pc = 0x200;
	m_Opcode = 0;
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
}

void CPU::RunCycle()
{
	// fetch opcode
	m_Opcode = (m_pMemory->FetchData(m_Pc) << 8u) | m_pMemory->FetchData(m_Pc + 1);
	// execute current opcode
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

// Clear screen
void CPU::Opcode00E0()
{
	// Clear screen
	if (m_pGraphics) m_pGraphics->Clear();
	m_Pc += 2;
}

// Return from subroutine
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
	// stack underflow or no memory: treat as error
	OpcodeNULL();
}

// 0x8 group implementations (arithmetic/logic)
// set vX to value of vY
void CPU::Opcode8xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	m_V[x] = m_V[y];
	m_Pc += 2;
}

// set vX to result of bitwise vX OR vY
void CPU::Opcode8xy1()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// According to spec, these logical ops clear VF
	m_V[0xF] = 0;
	m_V[x] |= m_V[y];
	m_Pc += 2;
}

// set vX to result of bitwise vX AND vY
void CPU::Opcode8xy2()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// Logical AND clears VF
	m_V[0xF] = 0;
	m_V[x] &= m_V[y];
	m_Pc += 2;
}

// set vX to result of bitwise vX XOR vY
void CPU::Opcode8xy3()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// Logical XOR clears VF
	m_V[0xF] = 0;
	m_V[x] ^= m_V[y];
	m_Pc += 2;
}

// set vX to result of vX + vY, set VF to 1 if overflow, else 0
void CPU::Opcode8xy4()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// Read operands first so VF writes don't affect input operands
	unsigned short vx = m_V[x];
	unsigned short vy = m_V[y];
	unsigned short sum = vx + vy;
	unsigned char result = (unsigned char)(sum & 0xFF);
	// Write result first, then set VF so that VF is based on original operands
	m_V[x] = result;
	m_V[0xF] = (sum > 0xFF) ? 1 : 0;
	m_Pc += 2;
}

// set vX to result of vX - vY, set VF to 0 if underflow, else 1
void CPU::Opcode8xy5()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// Read operands first
	unsigned short vx = m_V[x];
	unsigned short vy = m_V[y];
	unsigned char result = (unsigned char)((vx - vy) & 0xFF);
	// Write result then set VF (VF = 1 if no borrow, i.e., vx >= vy)
	m_V[x] = result;
	m_V[0xF] = (vx >= vy) ? 1 : 0;
	m_Pc += 2;
}

// set vX to result of vY >> 1, set VF to LSB of original vY
void CPU::Opcode8xy6()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Traditionally some interpreters use Vx as source, others use Vy.
	// Use Vy as source here so VF can be used as Vy input in tests.
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	unsigned char vy = m_V[y];
	unsigned char result = vy >> 1;
	// Write result then set VF (LSB of original vy)
	m_V[x] = result;
	m_V[0xF] = vy & 0x1;
	m_Pc += 2;
}

// set vX to result of vY - vX, set VF to 0 if underflow, else 1
void CPU::Opcode8xy7()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	// Read operands first
	unsigned short vx = m_V[x];
	unsigned short vy = m_V[y];
	unsigned char result = (unsigned char)((vy - vx) & 0xFF);
	// Write result then set VF (VF = 1 if no borrow, i.e., vy >= vx)
	m_V[x] = result;
	m_V[0xF] = (vy >= vx) ? 1 : 0;
	m_Pc += 2;
}

// set vX to result of vY << 1, set VF to MSB of original vY
void CPU::Opcode8xyE()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Use Vy as source so VF can be used as Vy input in tests
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	unsigned char vy = m_V[y];
	unsigned char result = (unsigned char)((vy << 1) & 0xFF);
	// Write result then set VF (MSB of original vy)
	m_V[x] = result;
	m_V[0xF] = (vy & 0x80) >> 7;
	m_Pc += 2;
}

// 0xE group (input related)
// Skip next instruction if key with the value of Vx is pressed
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

// Skip next instruction if key with the value of Vx is not pressed
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

// set Vx = delay timer value
void CPU::OpcodeFx07()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_V[x] = m_DelayTimer;
	m_Pc += 2;
}

// Wait for a key press AND release, store the value of the key in Vx
void CPU::OpcodeFx0A()
{
	// Wait for a key press, store the value of the key in Vx
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	if (m_pInput)
	{
		// If we're already waiting for a key release, check release first
		if (m_WaitingForKeyRelease)
		{
			// Wait until the previously recorded key is released
			if (!m_pInput->GetKeyState(m_WaitingKey))
			{
				// key released: stop waiting and advance PC
				m_WaitingForKeyRelease = false;
				m_WaitingKey = 0;
				m_WaitingKeyRegister = 0;
				m_Pc += 2;
			}
			// otherwise stay blocked
			return;
		}

		unsigned char pressedKey = 0;
		if (m_pInput->AnyKeyPressed(pressedKey))
		{
			// store pressed key into Vx and enter wait-for-release state
			m_V[x] = pressedKey;
			m_WaitingForKeyRelease = true;
			m_WaitingKey = pressedKey;
			m_WaitingKeyRegister = x;
		}
	}
	else
	{
		// no input handler: skip
		m_Pc += 2;
	}
}

// set delay timer = Vx
void CPU::OpcodeFx15()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_DelayTimer = m_V[x];
	m_Pc += 2;
}

// set sound timer = Vx
void CPU::OpcodeFx18()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_SoundTimer = m_V[x];
	m_Pc += 2;
}

// set I = I + Vx
void CPU::OpcodeFx1E()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	m_I = m_I + m_V[x];
	m_Pc += 2;
}

// set I = location of sprite for digit Vx (fontset)
void CPU::OpcodeFx29()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	// Each font character is 5 bytes and stored at 0x050
	m_I = 0x050 + (m_V[x] * 5);
	m_Pc += 2;
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2
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

// Store registers V0 through Vx in memory starting at address I
void CPU::OpcodeFx55()
{
	if (!m_pMemory) { OpcodeNULL(); return; }
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	for (unsigned char idx = 0; idx <= x; ++idx)
		m_pMemory->WriteData(m_I + idx, m_V[idx]);

	// Original CHIP-8 behavior: I is incremented by X + 1 after operation
	m_I = (unsigned short)(m_I + x + 1);
	m_Pc += 2;
}

// Read registers V0 through Vx from memory starting at address I
void CPU::OpcodeFx65()
{
	if (!m_pMemory) { OpcodeNULL(); return; }
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	for (unsigned char idx = 0; idx <= x; ++idx)
		m_V[idx] = m_pMemory->FetchData(m_I + idx);

	// Original CHIP-8 behavior: I is incremented by X + 1 after operation
	m_I = (unsigned short)(m_I + x + 1);
	m_Pc += 2;
}

// jump to address nnn
void CPU::Opcode1nnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_Pc = addr;
}

// push return address onto stack and jump to address nnn
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

// Skip next instruction if Vx == kk
void CPU::Opcode3xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	if (m_V[x] == kk) m_Pc += 4; else m_Pc += 2;
}

// Skip next instruction if Vx != kk
void CPU::Opcode4xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	if (m_V[x] != kk) m_Pc += 4; else m_Pc += 2;
}

// Skip next instruction if Vx == Vy
void CPU::Opcode5xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	if (m_V[x] == m_V[y]) m_Pc += 4; else m_Pc += 2;
}

// Set Vx = kk
void CPU::Opcode6xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = kk;
	m_Pc += 2;
}

// Set Vx = Vx + kk
void CPU::Opcode7xkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = m_V[x] + kk;
	m_Pc += 2;
}

// Skip next instruction if Vx != Vy
void CPU::Opcode9xy0()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char y = (m_Opcode & 0x00F0u) >> 4u;
	if (m_V[x] != m_V[y]) m_Pc += 4; else m_Pc += 2;
}

// Set I = nnn
void CPU::OpcodeAnnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_I = addr;
	m_Pc += 2;
}

// Jump to location nnn + V0
void CPU::OpcodeBnnn()
{
	unsigned short addr = m_Opcode & 0x0FFFu;
	m_Pc = (unsigned short)(m_V[0] + addr);
}

// Set Vx = random byte AND kk
void CPU::OpcodeCxkk()
{
	unsigned char x = (m_Opcode & 0x0F00u) >> 8u;
	unsigned char kk = m_Opcode & 0x00FFu;
	m_V[x] = (unsigned char)(std::rand() & kk);
	m_Pc += 2;
}

// Draw sprite at coordinate (Vx, Vy) with N bytes of sprite data starting at I
// has a lot of quirks on the CHIP-8 related to wrapping and clipping
void CPU::OpcodeDxyn()
{
	// Draw sprite at coordinate (Vx, Vy) with N bytes of sprite data starting at I
	if (!m_pGraphics || !m_pMemory) { OpcodeNULL(); return; } // graphics or memory not available
	unsigned char x = m_V[(m_Opcode & 0x0F00u) >> 8u];
	unsigned char y = m_V[(m_Opcode & 0x00F0u) >> 4u];
	unsigned char height = m_Opcode & 0x000Fu;
	// Wrap sprites: coordinates wrap around the screen edges (modulo 64x32)
	const unsigned char* screen = m_pGraphics->GetScreen();
	unsigned char screenW = m_pGraphics->GetWidth();
	unsigned char screenH = m_pGraphics->GetHeight();

	m_V[0xF] = 0;

	const unsigned int sx = (unsigned int)x;
	const unsigned int sy = (unsigned int)y;
	const bool wrapHoriz = sx >= screenW;
	const bool wrapVert = sy >= screenH;
	const unsigned int baseXWrapped = sx % screenW;
	const unsigned int baseYWrapped = sy % screenH;

	for (unsigned char row = 0; row < height; ++row)
	{
		const unsigned char spriteByte = m_pMemory->FetchData(m_I + row);
		if (spriteByte == 0) continue;

		// compute py once per row
		unsigned int py = wrapVert ? (baseYWrapped + (unsigned int)row) % screenH : (sy + (unsigned int)row);
		if (!wrapVert && py >= screenH) continue; // clip rows beyond bottom when not wrapping

		for (unsigned char bit = 0; bit < 8; ++bit)
		{
			if ((spriteByte & (0x80 >> bit)) == 0) continue;

			unsigned int px;
			if (wrapHoriz)
			{
				// wrap horizontally
				px = (baseXWrapped + (unsigned int)bit) % screenW;
			}
			else
			{
				// no horizontal wrap: clip pixels past right edge
				px = sx + (unsigned int)bit;
				if (px >= screenW) continue;
			}
			const unsigned int idx = py * screenW + px;
			if (screen[idx] != 0) m_V[0xF] = 1; // collision if pixel was set
			m_pGraphics->SetPixel((unsigned char)px, (unsigned char)py);
		}
	}
	m_Pc += 2;
}
