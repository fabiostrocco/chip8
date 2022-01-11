#include <gtest/gtest.h>

#include "clparser/CommandLineParser.hpp"
#include "clparser/CommandLineOptions.hpp"
#include "clparser/Required.hpp"
#include "clparser/Optional.hpp"

class EmptyTestOptions : public clparser::CommandLineOptions {};

class TestOptions : public clparser::CommandLineOptions
{
public:
	const std::string OptionalString = std::string("<default>");
	static constexpr uint8_t OptionalSmallDefaultValue = 10;
	static constexpr uint32_t OptionalUnsignedValue = 11;
	static constexpr int32_t OptionalSignedValue = -11;
	static constexpr float OptionalFloatValue = 1.0f;


	TestOptions() :
		requiredString(*this, "rst", "requiredString", "This is a required string"),
		requiredSmall(*this, "rsm", "requiredSmall", "This is a required small number"),
		requiredUnsigned(*this, "ru", "requiredUnsigned", "This is a required unsigned number"),
		requireSigned(*this, "rsg", "requiredSigned", "This is a required signed number"),
		requiredFloat(*this, "rf", "requiredFloat", "This is a required float number"),

		optionalString(*this, "ost", "optionalString", "This is an optional string", OptionalString),
		optionalSmall(*this, "osm", "optionalSmall", "This is an optional small number", OptionalSmallDefaultValue),
		optionalUnsigned(*this, "ou", "optionalUnsigned", "This is an optional unsigned number", OptionalUnsignedValue),
		optionalSigned(*this, "osg", "optionalSigned", "This is an optional signed number", OptionalSignedValue),
		optionalFloat(*this, "of", "optionalFloat", "This is an optional float number", OptionalFloatValue) {}

private:
	clparser::Required<std::string> requiredString;
	clparser::Required<uint8_t> requiredSmall;
	clparser::Required<uint32_t> requiredUnsigned;
	clparser::Required<int32_t> requireSigned;
	clparser::Required<float> requiredFloat;

	clparser::Optional<std::string> optionalString;
	clparser::Optional<uint8_t> optionalSmall;
	clparser::Optional<uint32_t> optionalUnsigned;
	clparser::Optional<int32_t> optionalSigned;
	clparser::Optional<float> optionalFloat;
};

namespace
{
	// This function will cause memory leaks, but it is acceptable since it is used in few tests.
	std::pair<int, char**> buildArgs(const std::vector<std::string> arguments)
	{
		std::pair<int, char**> args;

		args.first = arguments.size();
		args.second = new char* [arguments.size()];

		for (size_t i = 0; i < arguments.size(); i++)
		{
			const size_t stringSize = arguments[i].length();

			args.second[i] = new char[stringSize];
			memcpy(args.second[i], arguments[i].c_str(), stringSize);
		}

		return args;
	}
}

TEST(CommandLineParserTest, TestOne)
{
	auto args = buildArgs({ "--arg1", "val1", "--arg2", "val2" });

	clparser::CommandLineParser parser(args.first, args.second);
	EmptyTestOptions options = parser.parse<EmptyTestOptions>();

	EXPECT_EQ(options.getFields().size(), 0);
}
