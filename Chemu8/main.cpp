#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W = 800;     // window width  in pixels
const int   WINDOW_H = 600;     // window height in pixels
const float SQUARE_SZ = 80.0f;   // side length of the square
const float SPEED = 300.0f;  // movement speed in pixels per second

int main(int argc, char* argv[])
{
    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Create Window
    SDL_Window* window = SDL_CreateWindow(
        "Controllable Square",   // title shown in the title bar
        WINDOW_W, WINDOW_H,      // size of the window in pixels
        0                        // flags: 0 means a plain default window
    );
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Square State
    float x = (WINDOW_W - SQUARE_SZ) / 2.0f;
    float y = (WINDOW_H - SQUARE_SZ) / 2.0f;

    // setup for Delta time
    Uint64 lastTime = SDL_GetTicks();

    // Main Loop
    bool running = true;
    SDL_Event event;

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
        // Move the square
        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (keys[SDL_SCANCODE_W]) y -= SPEED * delta;
        if (keys[SDL_SCANCODE_S]) y += SPEED * delta;
        if (keys[SDL_SCANCODE_A]) x -= SPEED * delta;
        if (keys[SDL_SCANCODE_D]) x += SPEED * delta;
        // Clamp to window
        x = SDL_clamp(x, 0.0f, WINDOW_W - SQUARE_SZ);
        y = SDL_clamp(y, 0.0f, WINDOW_H - SQUARE_SZ);


        // Render
        // Clear the window to dark grey
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Draw the square in orange
        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
        SDL_FRect square = { x, y, SQUARE_SZ, SQUARE_SZ };
        SDL_RenderFillRect(renderer, &square);

        // Show the finished frame
        SDL_RenderPresent(renderer);
    }
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}