#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Memory.h"

const int   WINDOW_W = 800;     // window width  in pixels
const int   WINDOW_H = 600;     // window height in pixels
const float SQUARE_SZ = 80.0f;   // side length of the square
const float SPEED = 300.0f;  // movement speed in pixels per second

bool initSDL(SDL_Window* window, SDL_Renderer* renderer)
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

int main(int argc, char* argv[])
{
    // create memory
    Memory memory = Memory();

    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!initSDL(window, renderer))
        return 1; // SDL failed init

    // setup for Delta time
    Uint64 lastTime = SDL_GetTicks();

    // Main Loop
    bool running = true;
    SDL_Event event;

    

    // print out RAM dump to console for debugging memory initialization
#ifdef _DEBUG
    memory.PrintRAM(0, 0xFFF);
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
        // Clear the window to dark grey
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Show the finished frame
        SDL_RenderPresent(renderer);
    }
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}