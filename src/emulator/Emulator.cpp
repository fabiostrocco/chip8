#include "Emulator.hpp"

#include <SDL.h>

chip8::Emulator::Emulator(const logging::Logger& logger, const std::string& romFileName)
    : logger(logger)
    , romFileName(romFileName)
    , window(logger)
{
}

void chip8::Emulator::run()
{
    window.run();
}