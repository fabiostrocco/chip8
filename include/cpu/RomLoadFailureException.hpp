#pragma once

#include <stdexcept>

namespace chip8
{
    class RomLoadFailureException : public std::runtime_error
    {
      public:
        explicit RomLoadFailureException(const std::string& message)
            : std::runtime_error("Failed to load ROM: " + message)
        {
        }
    };
}