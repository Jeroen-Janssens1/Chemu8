#include "Renderer.h"


Renderer::Renderer()
{

}

Renderer::~Renderer()
{
    // clean up SDL related
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
}

bool Renderer::InitializeRenderer()
{
    // Create Window
    m_pWindow = SDL_CreateWindow(
        "Controllable Square",   // title shown in the title bar
        m_WindowWidth, m_WindowHeight,      // size of the window in pixels
        0                        // flags: 0 means a plain default window
    );
    if (!m_pWindow)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create Renderer
    m_pRenderer = SDL_CreateRenderer(m_pWindow, nullptr);
    if (!m_pRenderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(m_pWindow);
        SDL_Quit();
        return false;
    }
    return true;
}

void Renderer::Draw()
{
    // Clear the window to dark grey
    SDL_SetRenderDrawColor(m_pRenderer, 30, 30, 30, 255);
    SDL_RenderClear(m_pRenderer);

    // Show the finished frame
    SDL_RenderPresent(m_pRenderer);
}