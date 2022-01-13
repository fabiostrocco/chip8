#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "clparser/CommandLineParser.hpp"
#include "clparser/CommandLineOptions.hpp"
#include "clparser/NamedArgument.hpp"
#include "clparser/PositionalArgument.hpp"
#include "clparser/ICommandLineOptions.hpp"
#include "clparser/Argument.hpp"

class EmptyTestOptions : public clparser::CommandLineOptions {};

class TestOptions : public clparser::CommandLineOptions
{
public:
	static constexpr uint32_t OptionalNumberPositional = 10;
	static constexpr uint32_t OptionalNumberNamed = 20;

	TestOptions() :
		clparser::CommandLineOptions(),
		positionalString(*this, "positionalString", clparser::required<std::string>()),
		positionalNumber(*this, "positionalNumber", clparser::optional<uint32_t>(OptionalNumberPositional)),
		requiredString(*this, "rs", "requiredString", "This is a required string", clparser::required<std::string>()),
		requiredNumber(*this, "rn", "requiredNumber", "This is a required number", clparser::required<uint32_t>()),
		optionalNumber(*this, "on", "optionalNumber", "This is an optional number", clparser::optional<uint32_t>(OptionalNumberNamed))
	{
	}

public:
	const clparser::PositionalArgument<std::string> positionalString;
	const clparser::PositionalArgument<uint32_t> positionalNumber;

	const clparser::NamedArgument<std::string> requiredString;
	const clparser::NamedArgument<uint32_t> requiredNumber;
	const clparser::NamedArgument<uint32_t> optionalNumber;
};

namespace
{
	// This function will cause memory leaks, but it is acceptable since it is used in few tests.
	std::pair<int, char**> buildArgs(const std::vector<std::string> arguments)
	{
		std::pair<int, char**> args;

		args.first = static_cast<int>(arguments.size());
		args.second = new char* [arguments.size()];

		for (size_t i = 0; i < arguments.size(); i++)
		{
			const size_t stringSize = arguments[i].length();

			args.second[i] = new char[stringSize + 1];
			memcpy(args.second[i], arguments[i].c_str(), stringSize);
			args.second[i][stringSize] = '\0';
		}

		return args;
	}
}

namespace clparser::integration_tests
{
	TEST(CommandLineParserIntegrationTests, NamedArgument_ParseEmptyOptionsWithEmptyArguments_Succeeds)
	{
		auto args = buildArgs({ });

		clparser::CommandLineParser parser(args.first, args.second);
		EmptyTestOptions options;
		parser.parse(options);
	}

	TEST(CommandLineParserIntegrationTests, NamedArgument_ParseTestOptionsWithAllShortArguments_Succeeds)
	{
		auto args = buildArgs({ "<dummy>", "mystring", "10", "-rs", "<value>", "-rn", "20", "-on", "30" });

		clparser::CommandLineParser parser(args.first, args.second);
		TestOptions options;
		parser.parse(options);

		EXPECT_EQ(options.positionalString(), "mystring");
		EXPECT_EQ(options.positionalNumber(), 10);
		EXPECT_EQ(options.requiredString(), "<value>");
		EXPECT_EQ(options.requiredNumber(), 20);
		EXPECT_EQ(options.optionalNumber(), 30);
	}

	TEST(CommandLineParserIntegrationTests, NamedArgument_ParseTestOptionsWithAllLongArguments_Succeeds)
	{
		auto args = buildArgs({ "<dummy>", "mystring", "10", "--requiredString", "<value>", "--requiredNumber", "20", "--optionalNumber", "30" });

		clparser::CommandLineParser parser(args.first, args.second);
		TestOptions options;
		parser.parse(options);

		EXPECT_EQ(options.positionalString(), "mystring");
		EXPECT_EQ(options.positionalNumber(), 10);
		EXPECT_EQ(options.requiredString(), "<value>");
		EXPECT_EQ(options.requiredNumber(), 20);
		EXPECT_EQ(options.optionalNumber(), 30);
	}

	TEST(CommandLineParserIntegrationTests, NamedArgument_ParseTestOptionsWithRequiredArgumentsOnly_Succeeds)
	{
		auto args = buildArgs({ "<dummy>", "mystring", "--requiredString", "<value>", "--requiredNumber", "20" });

		clparser::CommandLineParser parser(args.first, args.second);
		TestOptions options;
		parser.parse(options);

		EXPECT_EQ(options.positionalString(), "mystring");
		EXPECT_EQ(options.positionalNumber(), TestOptions::OptionalNumberPositional);
		EXPECT_EQ(options.requiredString(), "<value>");
		EXPECT_EQ(options.requiredNumber(), 20);
		EXPECT_EQ(options.optionalNumber(), TestOptions::OptionalNumberNamed);
	}

	TEST(CommandLineParserIntegrationTests, NamedArgument_TestOptionsPrintHelpGuides_CorrectString)
	{
		constexpr char ProgramName[] = "myprogram";

		TestOptions options;
		std::string help = options.getHelpMessage(ProgramName);
		std::vector<std::string> helpList;

		while (!help.empty())
		{
			size_t endOfLine = help.find("\n");
			std::string line = help.substr(0, endOfLine);

			if (!line.empty())
			{
				helpList.push_back(line);
			}

			help = help.substr(endOfLine + 1, help.size());
		}
		
		EXPECT_EQ(helpList.size(), 5);
		EXPECT_EQ(helpList[0], "Usage: myprogram positionalString [positionalNumber] -rs value -rn value [-on value] ");
		EXPECT_EQ(helpList[1], "Options");
		EXPECT_EQ(helpList[2], "\t--requiredString, -rs\tThis is a required string");
		EXPECT_EQ(helpList[3], "\t--requiredNumber, -rn\tThis is a required number");
		EXPECT_EQ(helpList[4], "\t--optionalNumber, -on\tThis is an optional number (Default value: 20)");
	}
}