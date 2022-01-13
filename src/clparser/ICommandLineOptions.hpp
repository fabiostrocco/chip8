#pragma once

#include <vector>

#include "IArgument.hpp"

namespace clparser
{
	class ICommandLineOptions
	{
	public:
		virtual void addArgument(clparser::IArgument& argument) = 0;
		virtual std::vector<clparser::IArgument*> getArguments() const = 0;
		virtual std::string getHelpMessage(const std::string& programName) const = 0;
	};
}