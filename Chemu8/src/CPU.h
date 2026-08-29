#pragma once
class CPU final
{
public:
	CPU();

	void RunCycle();
	
private:
	unsigned short m_Opcode;
	unsigned char m_V[16]{}; // the 15 V registers of the CPU + carry flag register
	unsigned short I{}; // Index register
	unsigned short pc{}; // program counter
	unsigned char delay_timer{}; // both these timers count at 60 Hz
	unsigned char sound_timer{}; // if this is 0, buzzer makes noise

};

