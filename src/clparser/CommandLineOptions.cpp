#include "CommandLineOptions.hpp"

#include <sstream>

#include "Argument.hpp"

void clparser::CommandLineOptions::addArgument(clparser::IArgument& argument)
{
	argument.accept(validator);
	arguments.emplace_back(&argument);
}

std::vector<clparser::IArgument*> clparser::CommandLineOptions::getArguments() const
{
	return arguments;
}

std::string clparser::CommandLineOptions::getHelpMessage(const std::string& programName) const
{
	HelpMessageBuilder messageBuilder;
	std::string helpMessage;
	std::stringstream ss;

	for (IArgument* argument : arguments)
	{
		argument->accept(messageBuilder);
	}

	ss << "Usage: " << programName << " " << messageBuilder.getArguments() << std::endl;
	ss << std::endl;
	ss << "Options" << std::endl;
	ss << messageBuilder.getOptionGuides() << std::endl;

	helpMessage = ss.str();
	return helpMessage;
}

void clparser::CommandLineOptions::HelpMessageBuilder::visitNamedArgument(
	const std::string& shortName,
	const std::string& longName,
	const std::string& description,
	const bool isFlag,
	const std::optional<std::string> defaultValue)
{
	if (defaultValue.has_value())
	{
		argumentStream << "[";
	}

	argumentStream << clparser::ShortNamePrefix << shortName;

	if (!isFlag)
	{
		argumentStream << " value";
	}

	if (defaultValue.has_value())
	{
		argumentStream << "]";
	}

	argumentStream << " ";

	guideStream << "\t" << clparser::LongNamePrefix << longName << ", " << clparser::ShortNamePrefix << shortName << "\t" << description;

	if (defaultValue.has_value())
	{
		guideStream << " (Default value: " << *defaultValue << ")";
	}

	guideStream << std::endl;
}

void clparser::CommandLineOptions::HelpMessageBuilder::visitPositionalArgument(const std::string& name, const size_t position, const std::optional<std::string> defaultValue)
{
	if (defaultValue.has_value())
	{
		argumentStream << "[";
	}

	argumentStream << name;

	if (defaultValue.has_value())
	{
		argumentStream << "]";
	}

	argumentStream << " ";
}

std::string clparser::CommandLineOptions::HelpMessageBuilder::getArguments() const
{
	return argumentStream.str();
}

std::string clparser::CommandLineOptions::HelpMessageBuilder::getOptionGuides() const
{
	return guideStream.str();
}

void clparser::CommandLineOptions::Validator::visitNamedArgument(
	const std::string& shortName,
	const std::string& longName,
	const std::string& description,
	const bool isFlag,
	const std::optional<std::string> defaultValue)
{
	hasAddedNamedArgument = true;
}

void clparser::CommandLineOptions::Validator::visitPositionalArgument(const std::string& name, const size_t position, const std::optional<std::string> defaultValue)
{
	if (hasAddedNamedArgument)
	{
		throw std::logic_error("Cannot add positional argument after named argument");
	}
}
