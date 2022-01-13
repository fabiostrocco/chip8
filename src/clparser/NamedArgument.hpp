#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <optional>

#include "ICommandLineOptions.hpp"
#include "Argument.hpp"
#include "ArgumentFormatException.hpp"
#include "ArgumentNotFoundException.hpp"
#include "IArgumentVisitor.hpp"

namespace clparser
{
	template <typename ArgumentType>
		requires IsStreammable<ArgumentType>
	class NamedArgument : public clparser::Argument<ArgumentType>
	{
	public:
		NamedArgument(
			clparser::ICommandLineOptions& options,
			const std::string& shortName,
			const std::string& longName,
			const std::string& description,
			const std::optional<ArgumentType> defaultValue) :

			Argument<ArgumentType>(options, defaultValue),
			shortName(shortName),
			longName(longName),
			description(description)
		{
			options.addArgument(*this);
		}

		void accept(clparser::IArgumentVisitor& visitor) override
		{
			auto defaultValueString = clparser::Argument<ArgumentType>::getDefaultValueString();
			visitor.visitNamedArgument(shortName, longName, description, std::is_same<ArgumentType, bool>::value, defaultValueString);
		}

	protected:
		void internalMatch(const std::vector<std::string>& arguments) override
		{
			ArgumentType storage;

			auto it = std::find_if(
				arguments.begin(),
				arguments.end(),
				[&](const std::string& arg) { return arg == ShortNamePrefix + shortName || arg == LongNamePrefix + longName; });

			const std::string longNameComplete = LongNamePrefix + longName;

			// For boolean flags: if argument has been found, set storage to true, otherwise false
			if constexpr (std::is_same<ArgumentType, bool>::value)
			{
				storage = (it != arguments.end());
				clparser::Argument<ArgumentType>::setStorage(storage);
				return;
			}

			if (it == arguments.end())
			{
				throw clparser::ArgumentNotFoundException(longNameComplete);
			}

			const size_t indexOfParameter = it - arguments.begin();

			if (indexOfParameter + 1 >= arguments.size())
			{
				throw clparser::ArgumentFormatException(longNameComplete, "<empty>");
			}

			const std::string value = arguments[indexOfParameter + 1];

			if (clparser::isArgumentName(value))
			{
				throw clparser::ArgumentFormatException(longNameComplete, value);
			}

			std::stringstream ss(value);
			if (!(ss >> storage))
			{
				throw clparser::ArgumentFormatException(longNameComplete, value);
			}

			clparser::Argument<ArgumentType>::setStorage(storage);
		}

	private:
		const std::string shortName;
		const std::string longName;
		const std::string description;
	};
}