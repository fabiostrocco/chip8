#pragma once

#include <cstdint>

namespace chip8
{
    class IGpu
    {
      public:
        virtual void clear() = 0;
        virtual bool setSprite(const size_t x, const size_t y, const std::vector<uint8_t>& sprite) = 0;
        virtual const std::vector<bool>& getFrameBuffer() = 0;

        virtual constexpr size_t getWidth() = 0;
        virtual constexpr size_t getHeight() = 0;
    };
}