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

        void run();

      private:
        const logging::Logger& logger;
    };
}