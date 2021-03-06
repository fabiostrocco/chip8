#include <SDL.h>
#include <cpu/Cpu.hpp>
#include <cpu/Gpu.hpp>
#include <cpu/RomLoadFailureException.hpp>
#include <cpu/Timer.hpp>
#include <emulator/Emulator.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "EmulatorWindow.hpp"

chip8::Emulator::Emulator(const logging::Logger& logger, const std::string& romFileName)
    : logger(logger)
    , romFileName(romFileName)
{
    logger.logInfo("Initializing SDL...");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    logger.logInfo("SDL initialized.");
}

chip8::Emulator::~Emulator()
{
    logger.logInfo("Shutting down SDL...");
    SDL_Quit();
    logger.logInfo("SDL shut down.");
}

#include "AudioController.hpp"

void chip8::Emulator::run(const std::string& programName, const std::string& version, const uint32_t clock)
{
    chip8::Gpu gpu(logger);
    chip8::Timer soundTimer;
    chip8::Timer delayTimer;
    chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);

    std::unique_ptr<std::ifstream> romData = loadRom();
    cpu.boot(*romData);

    chip8::EmulatorWindow window(logger, cpu, programName, version, clock);
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
