#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "ArgumentConcept.hpp"
#include "ArgumentFormatException.hpp"
#include "ArgumentNotFoundException.hpp"
#include "IArgument.hpp"
#include "ICommandLineOptions.hpp"

namespace clparser
{
    template <clparser::ArgumentConcept ArgumentType>
    class Argument : public clparser::IArgument
    {
      protected:
        Argument(clparser::ICommandLineOptions& options, std::optional<ArgumentType> defaultValue) requires(!std::is_same<ArgumentType, bool>::value)
            : storage(std::nullopt)
            , defaultValue(defaultValue)
            , index(options.getArguments().size())
        {
        }

        Argument(clparser::ICommandLineOptions& options) requires(std::is_same<ArgumentType, bool>::value)
            : storage(std::nullopt)
            , defaultValue(std::nullopt)
            , index(options.getArguments().size())
        {
        }

        virtual void internalMatch(const std::vector<std::string>& arguments) = 0;

      public:
        void match(const std::vector<std::string>& arguments) override
        {
            storage = std::nullopt;

            try
            {
                internalMatch(arguments);
            }
            catch (ArgumentNotFoundException& e)
            {
                if (defaultValue.has_value())
                {
                    setStorage(*defaultValue);
                }
                else
                {
                    throw e;
                }
            }

            // If internalMatch terminates without exception, storage must be initialized.
            if (!storage.has_value())
            {
                throw std::logic_error("Uninitialized successfully matched field: setStorage() must be called when a field is sucecssfully matched");
            }
        }

        ArgumentType operator()() const
        {
            if (!storage.has_value())
            {
                throw std::logic_error("Unanitialized field");
            }

            return *storage;
        }

      protected:
        void setStorage(const ArgumentType& value)
        {
            storage = value;
        }

        size_t getIndex()
        {
            return index;
        }

        std::optional<ArgumentType> getDefaultValue()
        {
            return defaultValue;
        }

        std::optional<std::string> getDefaultValueString()
        {
            auto defaultValue = getDefaultValue();
            std::optional<std::string> defaultValueString;
            if (defaultValue.has_value())
            {
                if constexpr (std::is_same<ArgumentType, std::string>::value)
                {
                    defaultValueString = *defaultValue;
                }
                else
                {
                    defaultValueString = std::to_string(*defaultValue);
                }
            }
            else
            {
                defaultValueString = std::nullopt;
            }

            return defaultValueString;
        }

      private:
        Argument(const Argument<ArgumentType>&) = delete;
        Argument<ArgumentType>& operator=(const Argument<ArgumentType>&) = delete;

      private:
        std::optional<ArgumentType> storage;
        std::optional<ArgumentType> defaultValue;
        size_t index;
    };

    static constexpr char ShortNamePrefix[] = "-";
    static constexpr char LongNamePrefix[] = "--";

    template <typename ArgumentType>
    std::optional<ArgumentType> required()
    {
        return {};
    }

    template <typename ArgumentType>
    std::optional<ArgumentType> optional(const ArgumentType& value)
    {
        return value;
    }

    static bool isArgumentName(const std::string& text)
    {
        std::stringstream ss(text);
        double buf;
        return !(ss >> buf) && (text.starts_with(ShortNamePrefix) || text.starts_with(LongNamePrefix));
    }
}