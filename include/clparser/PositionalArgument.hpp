#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "Argument.hpp"
#include "ArgumentConcept.hpp"
#include "ArgumentFormatException.hpp"
#include "ArgumentNotFoundException.hpp"
#include "ICommandLineOptions.hpp"

namespace clparser
{
    template <clparser::ArgumentConcept ArgumentType>
    class PositionalArgument : public clparser::Argument<ArgumentType>
    {
      public:
        PositionalArgument(clparser::ICommandLineOptions& options,
                           const std::string& name,
                           const std::optional<ArgumentType> defaultValue) requires(!std::is_same<ArgumentType, bool>::value)
            : Argument<ArgumentType>(options, defaultValue)
            , name(name)
        {
            options.addArgument(*this);
        }

        void accept(IArgumentVisitor& visitor) override
        {
            auto defaultValueString = clparser::Argument<ArgumentType>::getDefaultValueString();
            visitor.visitPositionalArgument(name, clparser::Argument<ArgumentType>::getIndex() + 1, defaultValueString);
        }

      protected:
        void internalMatch(const std::vector<std::string>& arguments) override
        {
            const size_t argsLength = arguments.size();
            size_t position = clparser::Argument<ArgumentType>::getIndex() + 1;

            if (position >= argsLength)
            {
                throw ArgumentNotFoundException(name);
            }

            const std::string value = arguments[position];

            if (clparser::isArgumentName(value))
            {
                throw ArgumentNotFoundException(name);
            }

            std::stringstream ss(value);
            ArgumentType storage;

            if (!(ss >> storage))
            {
                throw clparser::ArgumentFormatException(name, value);
            }

            clparser::Argument<ArgumentType>::setStorage(storage);
        }

      private:
        std::string name;
    };
}