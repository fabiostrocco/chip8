#pragma once

#include "logging/Logger.hpp"
#include "EmulatorWindow.hpp"

namespace chip8
{
    class Emulator
    {
      public:
        Emulator(const logging::Logger& logger, const std::string& romFileName);
        void run();

      private:
        const logging::Logger& logger;
        const std::string& romFileName;
        EmulatorWindow window;
    };
}