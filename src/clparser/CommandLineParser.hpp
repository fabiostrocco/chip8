#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "ICommandLineOptions.hpp"

namespace clparser
{
    class CommandLineParser
    {
      public:
        CommandLineParser(int argc, char** argv);
        bool parse(clparser::ICommandLineOptions& options);

      private:
        std::vector<std::string> arguments;
    };
}