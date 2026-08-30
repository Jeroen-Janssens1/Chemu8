#include "Renderer.h"
#include "Graphics.h"


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

    if (m_pGraphics && m_pRenderer)
    {
        const unsigned char* screen = m_pGraphics->GetScreen();
        unsigned char screenW = m_pGraphics->GetWidth();
        unsigned char screenH = m_pGraphics->GetHeight();

        // Determine integer pixel scale to fit screen
        int scaleX = m_WindowWidth / screenW;
        int scaleY = m_WindowHeight / screenH;
        int scale = scaleX < scaleY ? scaleX : scaleY;
        if (scale <= 0) scale = 1;

        // Center offset
        int offsetX = (m_WindowWidth - (screenW * scale)) / 2;
        int offsetY = (m_WindowHeight - (screenH * scale)) / 2;

        // Draw pixels
        SDL_FRect rectF;
        rectF.w = (float)scale;
        rectF.h = (float)scale;
        for (int y = 0; y < screenH; ++y)
        {
            for (int x = 0; x < screenW; ++x)
            {
                unsigned char p = screen[y * screenW + x];
                if (p)
                {
                    SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
                    rectF.x = (float)(offsetX + x * scale);
                    rectF.y = (float)(offsetY + y * scale);
                    SDL_RenderFillRect(m_pRenderer, &rectF);
                }
            }
        }
    }

    // Show the finished frame
    SDL_RenderPresent(m_pRenderer);
}