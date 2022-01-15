#pragma once

#include <optional>
#include <sstream>
#include <vector>

#include "IArgument.hpp"
#include "ICommandLineOptions.hpp"
#include "NamedArgument.hpp"

namespace clparser
{
    class CommandLineOptions : public clparser::ICommandLineOptions
    {
      public:
        void addArgument(clparser::IArgument& field) override;
        std::vector<clparser::IArgument*> getArguments() const override;
        std::string getHelpMessage(const std::string& programName) const override;
        bool matchHelpAndGuides(const std::vector<std::string>& arguments) override;

      protected:
        CommandLineOptions()
            : helpArgument(nullptr)
            , versionArgument(nullptr)
        {
        }

        CommandLineOptions(clparser::NamedArgument<bool>& helpArgument, clparser::NamedArgument<bool>& versionArgument)
            : helpArgument(&helpArgument)
            , versionArgument(&versionArgument)
        {
        }

      private:
        // Using reference wrappers or optional here would give problems with gcc 10
        // Unique pointers are ruled out too because copy constructor and assignments are deelted for Arguments, to make sure they
        // are not coped anywhere making original values out of sync. Revisit this in the future, maybe it is good to have copy
        // enabled and use reference wrappers and optional here.
        std::vector<clparser::IArgument*> arguments; // this could be vector<IArgument&>
        clparser::NamedArgument<bool>* helpArgument; // this could be optional<NamedArgument<bool>&>
        clparser::NamedArgument<bool>* versionArgument; // this could be optional<NamedArgument<bool>&>

        class HelpMessageBuilder : public clparser::IArgumentVisitor
        {
          public:
            void visitNamedArgument(const std::string& shortName,
                                    const std::string& longName,
                                    const std::string& description,
                                    const bool isFlag,
                                    const std::optional<std::string> defaultValue) override;

            void visitPositionalArgument(const std::string& name,
                                         const size_t position,
                                         const std::optional<std::string> defaultValue) override;

            std::string getArguments() const;
            std::string getOptionGuides() const;

          private:
            std::stringstream argumentStream;
            std::stringstream guideStream;
        };

        class Validator : public clparser::IArgumentVisitor
        {
          public:
            void visitNamedArgument(const std::string& shortName,
                                    const std::string& longName,
                                    const std::string& description,
                                    const bool isFlag,
                                    const std::optional<std::string> defaultValue) override;

            void visitPositionalArgument(const std::string& name,
                                         const size_t position,
                                         const std::optional<std::string> defaultValue) override;

          private:
            bool hasAddedNamedArgument = false;
        };

        Validator validator;
    };
}
