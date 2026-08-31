#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>

class InputHandler final
{
public:
	bool ProcessInput();
	unsigned char GetKeyState(unsigned char key) const { return m_InputKeys[key]; }
	bool AnyKeyPressed(unsigned char &outKey) const;

private:
	SDL_Event m_Event{};
	unsigned char m_InputKeys[16]{};
	std::unordered_map<SDL_Keycode, unsigned char> m_KeyMap{
		{SDLK_X, 0x0}, {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3},
		{SDLK_Q, 0x4}, {SDLK_W, 0x5}, {SDLK_E, 0x6}, {SDLK_A, 0x7},
		{SDLK_S, 0x8}, {SDLK_D, 0x9}, {SDLK_Z, 0xA}, {SDLK_C, 0xB},
		{SDLK_4, 0xC}, {SDLK_R, 0xD}, {SDLK_F, 0xE}, {SDLK_V, 0xF}
	};

};

