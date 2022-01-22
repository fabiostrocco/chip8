#pragma once

#include <cstdint>

namespace chip8
{
    class IBinder
    {
      public:
        virtual bool match(const uint8_t byte1, const uint8_t byte2) = 0;
    };
}