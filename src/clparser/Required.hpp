#pragma once

#include "Field.hpp"
#include "CommandLineOptions.hpp"

namespace clparser
{
	template <typename FieldType>
	class Required : public clparser::Field<FieldType>
	{
	public:
		Required(clparser::CommandLineOptions& options, const std::string& shortName, const std::string& longName, const std::string& description) :
			clparser::Field<FieldType>(options, shortName, longName, description) {}
	};
}