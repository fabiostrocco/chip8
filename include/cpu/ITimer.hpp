#pragma once

#include <cstdint>

namespace chip8
{
    class ITimer
    {
      public:
        virtual void setValue(const uint8_t value) = 0;
        virtual uint8_t getValue() const = 0;
        virtual void updateValue() = 0;
    };
}