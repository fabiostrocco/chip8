#pragma once

#include <cstdint>
#include <vector>

#include "IGpu.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class Gpu : public IGpu
    {
      public:
        Gpu(const logging::Logger& log);

        void clear() override;
        bool setSprite(const size_t x, const size_t y, const std::vector<uint8_t>& sprite) override;
        const std::vector<bool>& getFrameBuffer() override;

        size_t getWidth() override;
        size_t getHeight() override;

      private:
        const logging::Logger& logger;
        std::vector<bool> frameBuffer;
    };
}