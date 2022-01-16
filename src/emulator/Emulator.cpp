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
    logger.logDebug("Initializing SDL...");
    SDL_Init(SDL_INIT_VIDEO);
    logger.logDebug("SDL initialized.");
    window.run();
    SDL_Quit();
}