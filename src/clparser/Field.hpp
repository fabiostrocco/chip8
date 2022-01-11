#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <regex>

#include "CommandLineParser.hpp"
#include "IField.hpp"
#include "ArgumentFormatException.hpp"
#include "ArgumentNotFoundException.hpp"

namespace clparser
{
	template <typename FieldType>
	class Field : public IField
	{
	protected:
		Field(clparser::CommandLineOptions& options, const std::string& shortName, const std::string& longName, const std::string& description) :
			shortName(shortName),
			longName(longName),
			description(description)
		{
			options.addField(*this);
		}

		void setStorage(const FieldType& value)
		{
			storage = value;
		}

	public:
		void match(const std::vector<std::string>& arguments) override
		{
			auto it = std::find_if(
				arguments.begin(),
				arguments.end(),
				[&](const std::string& arg) { return arg == "--" + shortName || arg == "--" + longName; });

			if (it == arguments.end())
			{
				throw clparser::ArgumentNotFoundException(longName);
			}

			size_t indexOfParameter = it - arguments.begin();

			if (indexOfParameter + 1 >= arguments.size())
			{
				throw clparser::ArgumentNotFoundException(longName);
			}

			std::stringstream ss(arguments[indexOfParameter + 1]);
			if (!(ss >> storage))
			{
				throw clparser::ArgumentFormatException(longName);
			}
		}

	private:
		const std::string shortName;
		const std::string longName;
		const std::string description;
		FieldType storage;
	};
}