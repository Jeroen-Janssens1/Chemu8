#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Memory.h"
#include "Renderer.h"

int main(int argc, char* argv[])
{
    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // create memory, renderer
    Memory* memory = new Memory();
    Renderer* renderer = new Renderer();


    if (!renderer->InitializeRenderer())
        return 1; // SDL failed init

    // setup for Delta time
    Uint64 lastTime = SDL_GetTicks();

    // Main Loop
    bool running = true;
    SDL_Event event;

    // print out RAM dump to console for debugging memory initialization
#ifdef _DEBUG
    memory->PrintRAM(0, 0xFFF);
#endif

    while (running)
    {
        // Delta time
        Uint64 now = SDL_GetTicks();
        float  delta = (now - lastTime) / 1000.0f;  // seconds
        lastTime = now;

        // SDL Event polling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
                running = false;
        }
        
        // Update


        // Render
        renderer->Draw();
    }
    // Cleanup
    delete renderer;
    delete memory;
    SDL_Quit();

    return 0;
}