#include "cpu/Gpu.hpp"

chip8::Gpu::Gpu(const logging::Logger& logger)
    : logger(logger)
    , frameBuffer(getWidth() * getHeight())
{
}

void chip8::Gpu::clear()
{
    std::fill(frameBuffer.begin(), frameBuffer.end(), 0);
}

bool chip8::Gpu::setSprite(const size_t x, const size_t y, const std::vector<uint8_t>& sprite)
{
    size_t currentLine = y;
    static constexpr size_t PixelsPerRow = sizeof(uint8_t) * 8;
    bool anyPixelErased = false;

    for (const uint8_t row : sprite)
    {
        // Cut the part of the sprite going out of screen
        if (currentLine >= getHeight())
        {
            return anyPixelErased;
        }

        for (size_t i = 0; i < PixelsPerRow; i++)
        {
            bool bit_i = (row >> (PixelsPerRow - i - 1)) % 2;
            const size_t bitLocationInScreen = (currentLine * getWidth() + ((x + i) % getWidth())); // Modulo getWidth() wraps around
            bool pixel = frameBuffer[bitLocationInScreen] ^ bit_i;
            if (!pixel && frameBuffer[bitLocationInScreen])
            {
                anyPixelErased = true;
            }

            frameBuffer[bitLocationInScreen] = pixel;
        }

        currentLine++;
    }

    return anyPixelErased;
}

const std::vector<bool>& chip8::Gpu::getFrameBuffer() const
{
    return frameBuffer;
}

size_t chip8::Gpu::getWidth()
{
    return 64;
}

size_t chip8::Gpu::getHeight()
{
    return 32;
}