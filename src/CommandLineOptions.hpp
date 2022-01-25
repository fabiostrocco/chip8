#pragma once

#include "clparser/Argument.hpp"
#include "clparser/CommandLineOptions.hpp"
#include "clparser/NamedArgument.hpp"
#include "clparser/PositionalArgument.hpp"

namespace chip8
{
    class CommandLineOptions : public clparser::CommandLineOptions
    {
      public:
        CommandLineOptions()
            : clparser::CommandLineOptions(help, version)
            , romName(*this, "rom", clparser::required<std::string>())
            , clock(*this, "c", "clock", "Number of instructions per second", clparser::optional<uint32_t>(1000))
            , verbose(*this, "ver", "verbose", "Displays all log message")
            , help(*this, "h", "help", "Displays this help")
            , version(*this, "v", "version", "Shows this program version")
        {
        }

        clparser::PositionalArgument<std::string> romName;
        clparser::NamedArgument<uint32_t> clock;
        clparser::NamedArgument<bool> verbose;
        clparser::NamedArgument<bool> help;
        clparser::NamedArgument<bool> version;
    };
}