#include "Renderer.h"


Renderer::Renderer()
{

}

Renderer::~Renderer()
{
    // clean up SDL related
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

bool Renderer::InitializeRenderer()
{
    // Create Window
    window = SDL_CreateWindow(
        "Controllable Square",   // title shown in the title bar
        WINDOW_W, WINDOW_H,      // size of the window in pixels
        0                        // flags: 0 means a plain default window
    );
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create Renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    return true;
}

void Renderer::Draw()
{
    // Clear the window to dark grey
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Show the finished frame
    SDL_RenderPresent(renderer);
}