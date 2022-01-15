#pragma once

#include "IFrameBuffer.hpp"
#include "IGpu.hpp"
#include "logging/Logger.hpp"

namespace chip8
{
    class Gpu : public IGpu
    {
      public:
        Gpu(const logging::Logger& log, IFrameBuffer& frameBuffer);

      private:
        const logging::Logger& logger;
        IFrameBuffer& frameBuffer;
    };
}