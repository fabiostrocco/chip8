#pragma once

#include <stdexcept>

namespace chip8
{
    class WindowInitializationException : public std::runtime_error
    {
      public:
        explicit WindowInitializationException(const std::string& message)
            : std::runtime_error("Error initializing window: " + message)
        {
        }
    };
}