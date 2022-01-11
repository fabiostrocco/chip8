#pragma once

#include <string>
#include <vector>

namespace clparser
{
	class IField
	{
	public:
		virtual void match(const std::vector<std::string>& arguments) = 0;
	};
}