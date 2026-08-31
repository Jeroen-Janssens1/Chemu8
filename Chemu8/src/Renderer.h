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
	void SetGraphics(class Graphics* graphics) { m_pGraphics = graphics; }

private:
	Graphics* m_pGraphics = nullptr;

private:
	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pSDLRenderer = nullptr;
	const int   m_WindowWidth = 1280;     // window width  in pixels
	const int   m_WindowHeight = 640;     // window height in pixels

};

