#pragma once

#include "CpuExecutionException.hpp"
#include "PointerRegister.hpp"

namespace chip8
{
    struct Registers
    {
      public:
        Registers(std::function<void(uint16_t)> memoryWriteValidator,
                  std::function<void(uint16_t)> memoryReadValidator,
                  std::function<void(uint8_t)> stackWriteValidator,
                  std::function<void(uint8_t)> stackReadValidator)
            : V()
            , I(memoryWriteValidator, memoryReadValidator)
            , PC(memoryWriteValidator, memoryReadValidator)
            , SP(stackWriteValidator, stackReadValidator)
        {
        }

        std::array<uint8_t, 16> V;                                                                      // General purpose registers
        PointerRegister<uint16_t, CpuErrorCode::AddressOutOfBound, CpuErrorCode::AddressOutOfBound> I;  // Pointer regiter
        PointerRegister<uint16_t, CpuErrorCode::AddressOutOfBound, CpuErrorCode::AddressOutOfBound> PC; // Program counter
        PointerRegister<uint8_t, CpuErrorCode::StackUnderflow, CpuErrorCode::StackOverflow> SP;         // Stack pointer
    };
}