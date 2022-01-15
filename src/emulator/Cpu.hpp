#pragma once

#include "ICpu.hpp"
#include "IGpu.hpp"
#include "Registers.hpp"
#include "ITimer.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class Cpu : public chip8::ICpu
    {
      public:
        Cpu(const logging::Logger& logger, chip8::IGpu& gpu, chip8::ITimer& timer1, chip8::ITimer& timer2);

      private:
        const logging::Logger& logger;

        chip8::Registers registers;
        chip8::IGpu& gpu;
        chip8::ITimer& timer1;
        chip8::ITimer& timer2;
    };
}