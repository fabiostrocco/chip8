#pragma once

#include <stdexcept>

namespace clparser
{
    class ArgumentNotFoundException : public std::runtime_error
    {
      public:
        explicit ArgumentNotFoundException(const std::string& argumentName)
            : std::runtime_error("Cannot find argument " + argumentName)
        {
        }
    };
}