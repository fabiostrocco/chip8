#pragma once

#include "InstructionBinder.hpp"

namespace chip8
{
    class InstructionSet
    {
      public:
        InstructionSet(std::function<void(chip8::BindingUnitType, chip8::BindingUnitType)> instr1Callback)
        {
            createInstruction<0xf, chip8::placeholder, 0x1, chip8::placeholder>(instr1Callback);
        }

        void matchInstruction(uint8_t byte1, uint8_t byte2)
        {
            for (auto& instruction : instructions)
            {
                instruction->match(byte1, byte2);
            }
        }

        template <chip8::BindingUnitType u1,
                  chip8::BindingUnitType u2,
                  chip8::BindingUnitType u3,
                  chip8::BindingUnitType u4,
                  typename T>
        void createInstruction(T f)
        {
            auto instruction = std::make_unique<chip8::InstructionBinder<u1, u2, u3, u4>>(f);
            instructions.push_back(std::move(instruction));
        }

      private:
        std::vector<std::unique_ptr<chip8::IBinder>> instructions;
    };
}