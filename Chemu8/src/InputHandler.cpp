#include "InputHandler.h"

bool InputHandler::ProcessInput()
{
    bool running = true;
    while (SDL_PollEvent(&m_Event)) {
        switch (m_Event.type) {
            case SDL_EVENT_QUIT: {
                running = false;
            } break;

            case SDL_EVENT_KEY_DOWN: {
                if (m_Event.key.key == SDLK_ESCAPE)
                    running = false;
                m_InputKeys[m_KeyMap[m_Event.key.key]] = 1;
            } break;
            case SDL_EVENT_KEY_UP: {
                m_InputKeys[m_KeyMap[m_Event.key.key]] = 0;
            } break;
        }
    }
    return running;
}

bool InputHandler::AnyKeyPressed(unsigned char &outKey) const
{
    for (unsigned char i = 0; i < 16; ++i)
    {
        if (m_InputKeys[i])
        {
            outKey = i;
            return true;
        }
    }
    return false;
}
