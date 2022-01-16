#include "Cpu.hpp"

chip8::Cpu::Cpu(const logging::Logger& logger, chip8::IGpu& gpu, chip8::ITimer& timer1, chip8::ITimer& timer2)
    : logger(logger)
    , registers()
    , gpu(gpu)
    , timer1(timer1)
    , timer2(timer2)
    , instructions([](auto, auto) {})
{
}