#pragma once

#include <stdexcept>

namespace chip8
{
    class AudioInitializationException : public std::runtime_error
    {
      public:
        explicit AudioInitializationException(const std::string& message)
            : std::runtime_error("Error initializing audio device: " + message)
        {
        }
    };
}