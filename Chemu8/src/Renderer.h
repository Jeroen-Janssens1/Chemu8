#pragma once
#include <SDL3/SDL.h>
class Renderer
{
public:
	Renderer();
	~Renderer();

	bool InitializeRenderer();
	void Draw();

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	const int   WINDOW_W = 800;     // window width  in pixels
	const int   WINDOW_H = 600;     // window height in pixels

};

