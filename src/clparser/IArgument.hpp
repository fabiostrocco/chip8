#pragma once

#include <string>
#include <vector>

#include "IArgumentVisitor.hpp"

namespace clparser
{
	class IArgument
	{
	public:
		virtual void match(const std::vector<std::string>& arguments) = 0;
		virtual void accept(clparser::IArgumentVisitor& visitor) = 0;
	};
}