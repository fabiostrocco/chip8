#include "Emulator.hpp"

#include <SDL.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "Cpu.hpp"
#include "Gpu.hpp"
#include "RomLoadFailureException.hpp"
#include "Timer.hpp"

chip8::Emulator::Emulator(const logging::Logger& logger, const std::string& romFileName)
    : logger(logger)
    , romFileName(romFileName)
{
    logger.logDebug("Initializing SDL...");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    logger.logDebug("SDL initialized.");
}

chip8::Emulator::~Emulator()
{
    logger.logDebug("Shutting down SDL...");
    SDL_Quit();
    logger.logDebug("SDL shut down...");
}

#include "AudioController.hpp"

void chip8::Emulator::run()
{
    chip8::Gpu gpu(logger);
    chip8::Timer soundTimer;
    chip8::Timer delayTimer;
    chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);

    std::unique_ptr<std::ifstream> romData = loadRom();
    cpu.boot(*romData);

    EmulatorWindow window(logger, cpu);
    window.run();
}

std::unique_ptr<std::ifstream> chip8::Emulator::loadRom() const
{
    std::unique_ptr<std::ifstream> romFile = std::make_unique<std::ifstream>(romFileName, std::ios::binary);
    if (romFile->fail())
    {
        throw chip8::RomLoadFailureException("Couldn't open ROM " + romFileName);
    }

    return std::move(romFile);
}
