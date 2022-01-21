#pragma once

#include <stdexcept>

namespace chip8
{
    class AudioException : public std::runtime_error
    {
      public:
        explicit AudioException(const std::string& message)
            : std::runtime_error("Error initializing audio device: " + message)
        {
        }
    };
}