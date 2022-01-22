#pragma once

#include <optional>

namespace clparser
{
    class IArgumentVisitor
    {
      public:
        virtual void visitNamedArgument(const std::string& shortName,
                                        const std::string& longName,
                                        const std::string& description,
                                        const bool isFlag,
                                        const std::optional<std::string> defaultValue) = 0;

        virtual void visitPositionalArgument(const std::string& name, const size_t position, const std::optional<std::string> defaultValue) = 0;
    };
}