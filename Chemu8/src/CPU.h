#pragma once
class Memory; // forward-declare to avoid heavy include
class Graphics; // forward-declare to avoid heavy include
class InputHandler; // forward-declare
class Audio; // forward-declare audio
class CPU final
{
public:
	CPU(Memory* memory, Graphics* graphics, InputHandler* input, Audio* audio);

	void RunCycle();

	// Timer update (call every frame with delta seconds)
	void UpdateTimers(float deltaSeconds);

	// Expose sound timer for audio subsystem
	unsigned char GetSoundTimer() const { return m_SoundTimer; }
	
private:
	unsigned short m_Opcode;
	unsigned char m_V[16]{}; // the 15 V registers of the CPU + carry flag register
	unsigned short m_I{}; // Index register
	unsigned short m_Pc{}; // program counter
	unsigned char m_DelayTimer{}; // both these timers count at 60 Hz
	unsigned char m_SoundTimer{}; // if this is 0, buzzer makes noise
	const float m_Interval = 1.0f / 60.0f; // 60Hz timer interval in seconds

	// Pointer to memory
	Memory* m_pMemory{};

	// Pointer to graphics
	Graphics* m_pGraphics{};

	// Timer accumulator (for 60Hz timer updates)
	float m_TimerAcc{};

	// Pointer to input handler
	InputHandler* m_pInput{};

	// Audio subsystem (managed by CPU)
	Audio* m_pAudio{};

	// Fx0A waiting state: when true CPU is waiting for the previously detected key to be released
	bool m_WaitingForKeyRelease{};
	unsigned char m_WaitingKey{}; // which key we are waiting to be released
	unsigned char m_WaitingKeyRegister{}; // which Vx register the pressed key was stored into


	// Opcode functions
	// Primary opcode group handlers (first nibble)
	void Opcode0();
	void Opcode1();
	void Opcode2();
	void Opcode3();
	void Opcode4();
	void Opcode5();
	void Opcode6();
	void Opcode7();
	void Opcode8();
	void Opcode9();
	void OpcodeA();
	void OpcodeB();
	void OpcodeC();
	void OpcodeD();
	void OpcodeE();
	void OpcodeF();

	// 0x0 sub-table
	void Opcode00E0(); // CLS
	void Opcode00EE(); // RET

	// 0x8 sub-table (arithmetic / logic)
	void Opcode8xy0();
	void Opcode8xy1();
	void Opcode8xy2();
	void Opcode8xy3();
	void Opcode8xy4();
	void Opcode8xy5();
	void Opcode8xy6();
	void Opcode8xy7();
	void Opcode8xyE();

	// 0xE sub-table
	void OpcodeEx9E();
	void OpcodeExA1();

	// 0xF sub-table (many opcodes indexed by lowest byte)
	void OpcodeFx07();
	void OpcodeFx0A();
	void OpcodeFx15();
	void OpcodeFx18();
	void OpcodeFx1E();
	void OpcodeFx29();
	void OpcodeFx33();
	void OpcodeFx55();
	void OpcodeFx65();

	// Other specific opcodes
	void Opcode1nnn();
	void Opcode2nnn();
	void Opcode3xkk();
	void Opcode4xkk();
	void Opcode5xy0();
	void Opcode6xkk();
	void Opcode7xkk();
	void Opcode9xy0();
	void OpcodeAnnn();
	void OpcodeBnnn();
	void OpcodeCxkk();
	void OpcodeDxyn();

	// null opcode / ignored opcode
	void OpcodeNULL();

	// Function tables
	using OpFunc = void (CPU::*)();
	OpFunc m_OpTable[16]{};
	OpFunc m_Table0[16]{};
	OpFunc m_Table8[16]{};
	OpFunc m_TableE[16]{};
	OpFunc m_TableF[256]{};


};

