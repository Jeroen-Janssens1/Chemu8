#pragma once
#include <SDL3/SDL.h>

class InputHandler final
{
public:
	bool ProcessInput();
	unsigned char GetKeyState(unsigned char key) const { return m_InputKeys[key]; }
	bool AnyKeyPressed(unsigned char &outKey) const;

private:
	SDL_Event m_Event;
	unsigned char m_InputKeys[16]{};

};

