#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "emulator/Gpu.hpp"
#include "logging/Logger.hpp"

class Logger : public logging::Logger
{
  public:
    Logger()
        : logging::Logger(logging::Severity::Debug)
    {
    }

  protected:
    void logInternal(const logging::Severity severity, const char* format, ...) const override
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
};

Logger loggerForGpu;

namespace chip8::unit_tests
{
    TEST(GpuUnitTests, Clear_DirtyFrameBuffer_ClearSuccessfully)
    {
        chip8::Gpu gpu(loggerForGpu);

        std::vector<bool>& frameBuffer = const_cast<std::vector<bool>&>(gpu.getFrameBuffer());
        frameBuffer[0] = true;
        frameBuffer[5] = true;

        gpu.clear();

        for (const bool pixel : frameBuffer)
        {
            ASSERT_FALSE(pixel);
        }
    }

    TEST(GpuUnitTests, SetSprite_EmptyFrameBuffer_CopyToFrameBufferCorrectly)
    {
        chip8::Gpu gpu(loggerForGpu);

        std::vector<uint8_t> sprite;
        sprite.push_back(0b10010110);
        sprite.push_back(0b01101001);

        const bool pixelErased = gpu.setSprite(5, 6, sprite);

        const std::vector<bool>& frameBuffer = gpu.getFrameBuffer();
        size_t lineStart = gpu.getWidth() * 6 + 5;

        ASSERT_TRUE(frameBuffer[lineStart]);
        ASSERT_FALSE(frameBuffer[lineStart + 1]);
        ASSERT_FALSE(frameBuffer[lineStart + 2]);
        ASSERT_TRUE(frameBuffer[lineStart + 3]);
        ASSERT_FALSE(frameBuffer[lineStart + 4]);
        ASSERT_TRUE(frameBuffer[lineStart + 5]);
        ASSERT_TRUE(frameBuffer[lineStart + 6]);
        ASSERT_FALSE(frameBuffer[lineStart + 7]);

        lineStart = gpu.getWidth() * 7 + 5;
        ASSERT_FALSE(frameBuffer[lineStart]);
        ASSERT_TRUE(frameBuffer[lineStart + 1]);
        ASSERT_TRUE(frameBuffer[lineStart + 2]);
        ASSERT_FALSE(frameBuffer[lineStart + 3]);
        ASSERT_TRUE(frameBuffer[lineStart + 4]);
        ASSERT_FALSE(frameBuffer[lineStart + 5]);
        ASSERT_FALSE(frameBuffer[lineStart + 6]);
        ASSERT_TRUE(frameBuffer[lineStart + 7]);

        ASSERT_FALSE(pixelErased);
    }

    TEST(GpuUnitTests, SetSprite_DirtyFrameBufferCollisions_CopyCorrectlyAndVFTrue)
    {
        chip8::Gpu gpu(loggerForGpu);

        std::vector<uint8_t> sprite;
        sprite.push_back(0b10010110);
        sprite.push_back(0b01101001);

        std::vector<bool>& frameBuffer = const_cast<std::vector<bool>&>(gpu.getFrameBuffer());
        frameBuffer[gpu.getWidth() * 6 + 5] = true;

        const bool pixelErased = gpu.setSprite(5, 6, sprite);

        size_t lineStart = gpu.getWidth() * 6 + 5;
        ASSERT_FALSE(frameBuffer[lineStart]);
        ASSERT_FALSE(frameBuffer[lineStart + 1]);
        ASSERT_FALSE(frameBuffer[lineStart + 2]);
        ASSERT_TRUE(frameBuffer[lineStart + 3]);
        ASSERT_FALSE(frameBuffer[lineStart + 4]);
        ASSERT_TRUE(frameBuffer[lineStart + 5]);
        ASSERT_TRUE(frameBuffer[lineStart + 6]);
        ASSERT_FALSE(frameBuffer[lineStart + 7]);

        lineStart = gpu.getWidth() * 7 + 5;
        ASSERT_FALSE(frameBuffer[lineStart]);
        ASSERT_TRUE(frameBuffer[lineStart + 1]);
        ASSERT_TRUE(frameBuffer[lineStart + 2]);
        ASSERT_FALSE(frameBuffer[lineStart + 3]);
        ASSERT_TRUE(frameBuffer[lineStart + 4]);
        ASSERT_FALSE(frameBuffer[lineStart + 5]);
        ASSERT_FALSE(frameBuffer[lineStart + 6]);
        ASSERT_TRUE(frameBuffer[lineStart + 7]);

        ASSERT_TRUE(pixelErased);
    }

    TEST(GpuUnitTests, SetSprite_DirtyFrameBufferNoCollision_CopyCorrectlyVFFalse)
    {
        chip8::Gpu gpu(loggerForGpu);

        std::vector<uint8_t> sprite;
        sprite.push_back(0b00010110);
        sprite.push_back(0b01101001);

        std::vector<bool>& frameBuffer = const_cast<std::vector<bool>&>(gpu.getFrameBuffer());
        frameBuffer[gpu.getWidth() * 6 + 5] = true;

        const bool pixelErased = gpu.setSprite(5, 6, sprite);

        size_t lineStart = gpu.getWidth() * 6 + 5;
        ASSERT_TRUE(frameBuffer[lineStart]);
        ASSERT_FALSE(frameBuffer[lineStart + 1]);
        ASSERT_FALSE(frameBuffer[lineStart + 2]);
        ASSERT_TRUE(frameBuffer[lineStart + 3]);
        ASSERT_FALSE(frameBuffer[lineStart + 4]);
        ASSERT_TRUE(frameBuffer[lineStart + 5]);
        ASSERT_TRUE(frameBuffer[lineStart + 6]);
        ASSERT_FALSE(frameBuffer[lineStart + 7]);

        lineStart = gpu.getWidth() * 7 + 5;
        ASSERT_FALSE(frameBuffer[lineStart]);
        ASSERT_TRUE(frameBuffer[lineStart + 1]);
        ASSERT_TRUE(frameBuffer[lineStart + 2]);
        ASSERT_FALSE(frameBuffer[lineStart + 3]);
        ASSERT_TRUE(frameBuffer[lineStart + 4]);
        ASSERT_FALSE(frameBuffer[lineStart + 5]);
        ASSERT_FALSE(frameBuffer[lineStart + 6]);
        ASSERT_TRUE(frameBuffer[lineStart + 7]);

        ASSERT_FALSE(pixelErased);
    }

     TEST(GpuUnitTests, SetSprite_EmptyFrameBufferWrapAround_CopyToFrameBufferCorrectly)
    {
        chip8::Gpu gpu(loggerForGpu);

        std::vector<uint8_t> sprite;
        sprite.push_back(0b10010110);
        sprite.push_back(0b01101001);

        const bool pixelErased = gpu.setSprite(gpu.getWidth() - 1, 0, sprite);

        const std::vector<bool>& frameBuffer = gpu.getFrameBuffer();
        
        size_t lineStart = gpu.getWidth() - 1;

        ASSERT_TRUE(frameBuffer[lineStart]);
        ASSERT_FALSE(frameBuffer[0]);
        ASSERT_FALSE(frameBuffer[1]);
        ASSERT_TRUE(frameBuffer[2]);
        ASSERT_FALSE(frameBuffer[3]);
        ASSERT_TRUE(frameBuffer[4]);
        ASSERT_TRUE(frameBuffer[5]);
        ASSERT_FALSE(frameBuffer[6]);

        lineStart = gpu.getWidth() + gpu.getWidth() - 1;
        ASSERT_FALSE(frameBuffer[lineStart]);
        ASSERT_TRUE(frameBuffer[gpu.getWidth()]);
        ASSERT_TRUE(frameBuffer[gpu.getWidth() + 1]);
        ASSERT_FALSE(frameBuffer[gpu.getWidth() + 2]);
        ASSERT_TRUE(frameBuffer[gpu.getWidth() + 3]);
        ASSERT_FALSE(frameBuffer[gpu.getWidth() + 4]);
        ASSERT_FALSE(frameBuffer[gpu.getWidth() + 5]);
        ASSERT_TRUE(frameBuffer[gpu.getWidth() + 6]);

        ASSERT_FALSE(pixelErased);
    }
}