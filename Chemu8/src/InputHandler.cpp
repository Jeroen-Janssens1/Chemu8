#include "InputHandler.h"

bool InputHandler::ProcessInput()
{
    bool quit = false;
    while (SDL_PollEvent(&m_Event)) {
        switch (m_Event.type) {
            case SDL_EVENT_QUIT: {
                quit = true;
            } break;

            case SDL_EVENT_KEY_DOWN: {
                switch (m_Event.key.key) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    } break;

                    case SDLK_X: {
                        m_InputKeys[0] = 1;
                    } break;

                    case SDLK_1: {
                        m_InputKeys[1] = 1;
                    } break;

                    case SDLK_2: {
                        m_InputKeys[2] = 1;
                    } break;

                    case SDLK_3: {
                        m_InputKeys[3] = 1;
                    } break;

                    case SDLK_Q: {
                        m_InputKeys[4] = 1;
                    } break;

                    case SDLK_W: {
                        m_InputKeys[5] = 1;
                    } break;

                    case SDLK_E: {
                        m_InputKeys[6] = 1;
                    } break;

                    case SDLK_A: {
                        m_InputKeys[7] = 1;
                    } break;

                    case SDLK_S: {
                        m_InputKeys[8] = 1;
                    } break;

                    case SDLK_D: {
                        m_InputKeys[9] = 1;
                    } break;

                    case SDLK_Z: {
                        m_InputKeys[0xA] = 1;
                    } break;

                    case SDLK_C: {
                        m_InputKeys[0xB] = 1;
                    } break;

                    case SDLK_4: {
                        m_InputKeys[0xC] = 1;
                    } break;

                    case SDLK_R: {
                        m_InputKeys[0xD] = 1;
                    } break;

                    case SDLK_F: {
                        m_InputKeys[0xE] = 1;
                    } break;

                    case SDLK_V: {
                        m_InputKeys[0xF] = 1;
                    } break;
                }
            } break;

            case SDL_EVENT_KEY_UP: {
                switch (m_Event.key.key) {
                    case SDLK_X: {
                        m_InputKeys[0] = 0;
                    } break;

                    case SDLK_1: {
                        m_InputKeys[1] = 0;
                    } break;

                    case SDLK_2: {
                        m_InputKeys[2] = 0;
                    } break;

                    case SDLK_3: {
                        m_InputKeys[3] = 0;
                    } break;

                    case SDLK_Q: {
                        m_InputKeys[4] = 0;
                    } break;

                    case SDLK_W: {
                        m_InputKeys[5] = 0;
                    } break;

                    case SDLK_E: {
                        m_InputKeys[6] = 0;
                    } break;

                    case SDLK_A: {
                        m_InputKeys[7] = 0;
                    } break;

                    case SDLK_S: {
                        m_InputKeys[8] = 0;
                    } break;

                    case SDLK_D: {
                        m_InputKeys[9] = 0;
                    } break;

                    case SDLK_Z: {
                        m_InputKeys[0xA] = 0;
                    } break;

                    case SDLK_C: {
                        m_InputKeys[0xB] = 0;
                    } break;

                    case SDLK_4: {
                        m_InputKeys[0xC] = 0;
                    } break;

                    case SDLK_R: {
                        m_InputKeys[0xD] = 0;
                    } break;

                    case SDLK_F: {
                        m_InputKeys[0xE] = 0;
                    } break;

                    case SDLK_V: {
                        m_InputKeys[0xF] = 0;
                    } break;
                }
            } break;
        }
    }
    return quit;
}