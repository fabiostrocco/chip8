#pragma once

#include <vector>
#include <sstream>

#include "IArgument.hpp"
#include "ICommandLineOptions.hpp"

namespace clparser
{
	class CommandLineOptions : public clparser::ICommandLineOptions
	{
	public:
		void addArgument(clparser::IArgument& field) override;
		std::vector<clparser::IArgument*> getArguments() const override;
		std::string getHelpMessage(const std::string& programName) const override;

	private:
		// I would have used std::reference_wrapper but I had some issues with Linux gcc compiler.
		std::vector<clparser::IArgument*> arguments;

		class HelpMessageBuilder : public clparser::IArgumentVisitor
		{
		public:
			void visitNamedArgument(
				const std::string& shortName,
				const std::string& longName,
				const std::string& description,
				const bool isFlag,
				const std::optional<std::string> defaultValue) override;

			void visitPositionalArgument(const std::string& name, const size_t position, const std::optional<std::string> defaultValue) override;

			std::string getArguments() const;
			std::string getOptionGuides() const;

		private:
			std::stringstream argumentStream;
			std::stringstream guideStream;
		};

		class Validator : public clparser::IArgumentVisitor
		{
		public:
			void visitNamedArgument(
				const std::string& shortName,
				const std::string& longName,
				const std::string& description,
				const bool isFlag,
				const std::optional<std::string> defaultValue) override;

			void visitPositionalArgument(const std::string& name, const size_t position, const std::optional<std::string> defaultValue) override;

		private:
			bool hasAddedNamedArgument = false;
		};

		Validator validator;
	};
}
