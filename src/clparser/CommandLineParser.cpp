#include "CommandLineParser.hpp"

#include <filesystem>

clparser::CommandLineParser::CommandLineParser(int argc, char** argv)
{
    arguments.reserve(argc);

    for (size_t i = 0; i < argc; i++)
    {
        arguments.emplace_back(argv[i]);
    }
}

bool clparser::CommandLineParser::parse(clparser::ICommandLineOptions& options)
{
    if (options.matchHelpAndGuides(arguments))
    {
        return true;
    }

    for (auto argumentDescriptor : options.getArguments())
    {
        argumentDescriptor->match(arguments);
    }

    return false;
}
