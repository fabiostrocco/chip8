#include "Gpu.hpp"

chip8::Gpu::Gpu(const logging::Logger& logger, IFrameBuffer& frameBuffer)
    : logger(logger)
    , frameBuffer(frameBuffer)
{
}