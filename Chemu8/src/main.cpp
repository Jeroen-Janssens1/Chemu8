#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Memory.h"
#include "Renderer.h"
#include "InputHandler.h"

int main(int argc, char* argv[])
{
    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    // initialize components
    Memory* pMemory = new Memory();
    Renderer* pRenderer = new Renderer();
    InputHandler* pInputHandler = new InputHandler();
    if (!pRenderer->InitializeRenderer())
        return 1; // SDL failed init
    // setup for Delta time
    Uint64 lastTime = SDL_GetTicks();
    // print out RAM dump to console for debugging memory initialization
#ifdef _DEBUG
    pMemory->PrintRAM(0, 0xFFF);
#endif
    // Main Loop
    bool running = true;
    SDL_Event event;
    while (running)
    {
        // Delta time
        Uint64 now = SDL_GetTicks();
        float  delta = (now - lastTime) / 1000.0f;  // seconds
        lastTime = now;
        // Input polling
        running = pInputHandler->ProcessInput();
        // Update CPU
        
        // Render
        pRenderer->Draw();
    }
    // Cleanup
    delete pRenderer;
    delete pMemory;
    SDL_Quit();
    return 0;
}