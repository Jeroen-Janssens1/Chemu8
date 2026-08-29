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
	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;
	const int   m_WindowWidth = 800;     // window width  in pixels
	const int   m_WindowHeight = 600;     // window height in pixels

};

