#pragma once
#include <SDL3/SDL.h>
class Renderer final
{
public:
	Renderer();
	~Renderer();
	// rule of 5, we only ever want 1 renderer anyways so simply disable move-and-copy-ability
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;


	bool InitializeRenderer();
	void Draw();

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	const int   WINDOW_W = 800;     // window width  in pixels
	const int   WINDOW_H = 600;     // window height in pixels

};

