#pragma once

#include <string>
#include <vector>

namespace clparser
{
	class CommandLineParser
	{
	public:
		CommandLineParser(int argc, char** argv);

		template<typename Options>
		Options parse()
		{
			// TODO: Option <: COmmandLineOptions
			Options options;
			for (auto field : options.getFields())
			{
				field->match(arguments);
			}

			return options;
		}

	private:
		std::vector<std::string> arguments;
	};
}