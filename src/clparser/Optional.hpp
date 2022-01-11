#pragma once

#include "Field.hpp"
#include "CommandLineOptions.hpp"
#include "ArgumentNotFoundException.hpp"

namespace clparser
{
	template <typename FieldType>
	class Optional : public clparser::Field<FieldType>
	{
	public:
		Optional(clparser::CommandLineOptions& options, const std::string& shortName, const std::string& longName, const std::string& description, const FieldType& defaultValue) :
			clparser::Field<FieldType>(options, shortName, longName, description),
			defaultValue(defaultValue) {}

		void match(const std::vector<std::string>& arguments) override
		{
			try
			{
				clparser::Field<FieldType>::match(arguments);
			}
			catch (clparser::ArgumentNotFoundException&)
			{
				clparser::Field<FieldType>::setStorage(defaultValue);
			}
		}

	private:
		const FieldType defaultValue;
	};
}