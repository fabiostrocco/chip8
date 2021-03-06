#pragma once

#include <SDL_render.h>
#include <cpu/Cpu.hpp>

#include "AudioController.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class EmulatorWindow
    {
      public:
        EmulatorWindow(const logging::Logger& logger, chip8::Cpu& cpu, const std::string& programName, const std::string& version, const uint32_t clock);
        ~EmulatorWindow();

        void run();

      private:
        void init(const std::string& programName, const std::string& version);
        bool processEvents();
        void runCpuCycle();
        void clearRenderer();
        void drawFrame();
        void presentFrame();

        // It is not possible to copy windows because copying the content of window and renderer pointers is not supported.
        // Moving this object could be supported but it is necessary here (move is automatically disabled by disabling copying).
        EmulatorWindow(const EmulatorWindow&) = delete;
        EmulatorWindow& operator=(const EmulatorWindow&) = delete;

      private:
        static constexpr uint32_t MinimumInstructionTimerPeriodMilliseconds = 10;

        const logging::Logger& logger;
        chip8::Cpu& cpu;
        chip8::AudioController audioController;

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* chip8ScreenTexture;

        bool needsDraw;
        uint32_t clock;
    };
}