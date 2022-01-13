#pragma once

#include <string>
#include <vector>

#include "CommandLineOptions.hpp"

#include <type_traits>
#include <utility>
#include <iostream>
#include <sstream>

namespace clparser
{
	class CommandLineParser
	{
	public:
		CommandLineParser(int argc, char** argv);
		void parse(clparser::CommandLineOptions& options);

	private:
		std::vector<std::string> arguments;
	};
}