#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "clparser/IArgument.hpp"
#include "clparser/ICommandLineOptions.hpp"
#include "clparser/NamedArgument.hpp"
#include "clparser/PositionalArgument.hpp"

class MockCommandLineOptions : public clparser::ICommandLineOptions
{
public:
	MOCK_METHOD(void, addArgument, (clparser::IArgument&));
	MOCK_METHOD(std::vector<clparser::IArgument*>, getArguments, (), (const));
	MOCK_METHOD(std::string, getHelpMessage, (const std::string&), (const));
};

namespace clparser::unit_tests
{
	TEST(CommandLineParserUnitTests, NamedArgument_MatchedRequiredStringArgument_Succeeds)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<std::string> requiredString(options, "short", "long", std::string(), clparser::required<std::string>());
		requiredString.match({ "-short", "value" });
		EXPECT_EQ(requiredString(), "value");
	}

	TEST(CommandLineParserUnitTests, NamedArgument_MatchedRequiredUnsignedNumericArgument_Succeeds)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<uint32_t> requiredUint(options, "short", "long", std::string(), clparser::required<uint32_t>());
		requiredUint.match({ "-short", "100" });
		EXPECT_EQ(requiredUint(), 100);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_MatchedRequiredSignedNumericArgument_Succeeds)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<int32_t> requiredInt(options, "short", "long", std::string(), clparser::required<int32_t>());
		requiredInt.match({ "-short", "-100" });
		EXPECT_EQ(requiredInt(), -100);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_MatchedRequiredFloatNumericArgument_Succeeds)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<float> requiredFloat(options, "short", "long", std::string(), clparser::required<float>());
		requiredFloat.match({ "-short", "0.0" });
		EXPECT_EQ(requiredFloat(), 0.0f);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_MatchedRequiredBoolArgument_ReturnsTrue)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<bool> requiredFlag(options, "short", "long", std::string(), clparser::required<bool>());
		requiredFlag.match({ "-short" });
		EXPECT_EQ(requiredFlag(), true);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_UnmatchedRequiredBoolArgument_ReturnsFalse)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<bool> requiredFlag(options, "short", "long", std::string(), clparser::required<bool>());
		requiredFlag.match({});
		EXPECT_EQ(requiredFlag(), false);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_UnmatchedRequiredArgument_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<std::string> requiredString(options, "short", "long", std::string(), clparser::required<std::string>());

		try
		{
			requiredString.match({ "-foo", "value" });
			FAIL();
		}
		catch (clparser::ArgumentNotFoundException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Cannot find argument --long");
		}
	}

	TEST(CommandLineParserUnitTests, NamedArgument_IllFormedRequiredNumbricArgument_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<uint32_t> requiredString(options, "short", "long", std::string(), clparser::required<uint32_t>());

		try
		{
			requiredString.match({ "--long", "AR" });
			FAIL();
		}
		catch (clparser::ArgumentFormatException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Wrong value for argument --long: Value AR is not valid");
		}
	}

	TEST(CommandLineParserUnitTests, NamedArgument_MatchedLongName_Succeeds)
	{
		const char Value[] = "value";

		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<std::string> requiredString(options, "short", "long", std::string(), clparser::required<std::string>());
		requiredString.match({ "--long", Value });
		EXPECT_EQ(requiredString(), Value);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_UnmatchedShortForLong_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<std::string> requiredString(options, "short", "long", std::string(), clparser::required<std::string>());

		try
		{
			requiredString.match({ "--short", "0" });
			FAIL();
		}
		catch (clparser::ArgumentNotFoundException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Cannot find argument --long");
		}
	}

	TEST(CommandLineParserUnitTests, NamedArgument_UnmatchedLongForShort_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<std::string> requiredString(options, "short", "long", std::string(), clparser::required<std::string>());

		try
		{
			requiredString.match({ "-long", "0" });
			FAIL();
		}
		catch (clparser::ArgumentNotFoundException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Cannot find argument --long");
		}
	}

	TEST(CommandLineParserUnitTests, NamedArgument_UnmatchedOptionalArgument_Succeeds)
	{
		constexpr uint32_t DefaultValue = 50;

		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		NamedArgument<uint32_t> optionalString(options, "short", "long", std::string(), clparser::optional<uint32_t>(DefaultValue));
		optionalString.match({ "--foo", "value" });
		EXPECT_EQ(optionalString(), DefaultValue);
	}

	TEST(CommandLineParserUnitTests, NamedArgument_IllFormedOptionalNumericArgument_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);

		NamedArgument<uint32_t> optionalString(options, "short", "long", std::string(), clparser::optional<uint32_t>(0));

		try
		{
			optionalString.match({ "-short", "AR" });
			FAIL();
		}
		catch (clparser::ArgumentFormatException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Wrong value for argument --long: value AR is not valid");
		}
	}

	TEST(CommandLineParserUnitTests, PositionalArgument_MatchedRequiredArgumentAtStart_Succeeds)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		PositionalArgument<std::string> positionalString(options, "positional", clparser::required<std::string>());
		positionalString.match({ "<dummy>", "name", "--foo", "--bar" });
		EXPECT_EQ(positionalString(), "name");
	}

	TEST(CommandLineParserUnitTests, PositionalArgument_UnmatchedRequiredArgumentAtStart_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		PositionalArgument<std::string> positionalString(options, "positional", clparser::required<std::string>());

		try
		{
			positionalString.match({ "<dummy>" });
		}
		catch (ArgumentNotFoundException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Cannot find argument positional");
		}
	}

	TEST(CommandLineParserUnitTests, PositionalArgument_UnmatchedRequiredArgumentAtEnd_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		PositionalArgument<std::string> positionalString(options, "positional", clparser::required<std::string>());

		try
		{
			positionalString.match({ "<dummy>", "--field", "name" });
		}
		catch (ArgumentNotFoundException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Cannot find argument positional");
		}
	}

	TEST(CommandLineParserUnitTests, PositionalArgument_IllFormedRequiredArgumentAtStart_Fails)
	{
		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		PositionalArgument<uint32_t> positionalString(options, "positional", clparser::required<uint32_t>());

		try
		{
			positionalString.match({ "<dummy>", "X", "--name" });
		}
		catch (ArgumentFormatException& e)
		{
			EXPECT_STRCASEEQ(e.what(), "Wrong value for argument positional: value X is not valid");
		}
	}

	TEST(CommandLineParserUnitTests, PositionalArgument_UnmatchedOptionalArgument_Succeeds)
	{
		constexpr char DefaultValue[] = "<default>";

		MockCommandLineOptions options;
		EXPECT_CALL(options, addArgument);
		EXPECT_CALL(options, getArguments);
		PositionalArgument<std::string> positionalString(options, "positional", clparser::optional<std::string>(DefaultValue));
		positionalString.match({ "<dummy>", "--flag" });
		EXPECT_EQ(positionalString(), std::string(DefaultValue));
	}
}