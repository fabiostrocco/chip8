#pragma once

#include <SDL.h>

#include "IFrameBuffer.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class EmulatorWindow : public IFrameBuffer
    {
      public:
        EmulatorWindow(const logging::Logger& logger);
        ~EmulatorWindow();

        void run();

      private:
        void init();
        bool processEvents();
        void loop();
        void clearRenderer();
        void drawFrame();
        void presentFrame();

        // It is not possible to copy windows because copying the content of window and renrerer pointers is not supported.
        // Moving this object could be supported but it is necessary here (move is automatically disabled by disabling copying).
        EmulatorWindow(const EmulatorWindow&) = delete;
        EmulatorWindow& operator=(const EmulatorWindow&) = delete;

      private:
        const logging::Logger& logger;
        SDL_Window* window;
        SDL_Renderer* renderer;

        //TODO:remove
        int diagonal;
    };
}