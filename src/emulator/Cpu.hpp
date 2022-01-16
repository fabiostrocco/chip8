#pragma once

#include <array>
#include <memory>

#include "ICpu.hpp"
#include "IGpu.hpp"
#include "ITimer.hpp"
#include "InstructionSet.hpp"
#include "Registers.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class Cpu : public chip8::ICpu
    {
      public:
        Cpu(const logging::Logger& logger, chip8::IGpu& gpu, chip8::ITimer& timer1, chip8::ITimer& timer2);

      private:
        static constexpr size_t MemorySize = 1024;

        const logging::Logger& logger;
        const chip8::InstructionSet instructions;

        chip8::Registers registers;
        std::array<uint8_t, MemorySize> memory;
        chip8::IGpu& gpu;
        chip8::ITimer& timer1;
        chip8::ITimer& timer2;
    };
}
