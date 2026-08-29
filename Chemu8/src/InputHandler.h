#pragma once
#include <SDL3/SDL.h>

class InputHandler final
{
public:
	bool ProcessInput();

private:
	SDL_Event m_Event;
	unsigned char m_InputKeys[16]{};

};

