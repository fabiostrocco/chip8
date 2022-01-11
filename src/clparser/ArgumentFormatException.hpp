#pragma once

#include <stdexcept>

namespace clparser
{
	class ArgumentFormatException : public std::runtime_error
	{
	public:
		explicit ArgumentFormatException(const std::string& argumentName) : std::runtime_error("Wrong value for argument " + argumentName) {}
	};
}