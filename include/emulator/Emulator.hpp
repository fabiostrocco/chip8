#pragma once

#include <memory>
#include <vector>

#include "logging/Logger.hpp"

namespace chip8
{
    class Emulator
    {
      public:
        Emulator(const logging::Logger& logger, const std::string& romFileName);
        ~Emulator();

        void run(const std::string& programName, const std::string& version, const uint32_t clock);

      private:
        Emulator(const Emulator&) = delete;
        Emulator& operator=(const Emulator&) = delete;

        std::unique_ptr<std::ifstream> loadRom() const;

        const logging::Logger& logger;
        const std::string romFileName;
    };
}
