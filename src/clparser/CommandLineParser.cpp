#include "CommandLineParser.hpp"

clparser::CommandLineParser::CommandLineParser(int argc, char** argv)
{
	arguments.reserve(argc);

	for (size_t i = 0; i < argc; i++)
	{
		arguments.emplace_back(argv[i]);
	}
}

void clparser::CommandLineParser::parse(clparser::CommandLineOptions& options)
{
	for (auto argumentDescriptor : options.getArguments())
	{
		argumentDescriptor->match(arguments);
	}
}
