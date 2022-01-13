#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <sstream>
#include <optional>

#include "ICommandLineOptions.hpp"
#include "IArgument.hpp"
#include "ArgumentFormatException.hpp"
#include "ArgumentNotFoundException.hpp"

namespace
{
	template<typename S, typename T>
	class is_streamable
	{
		template<typename SS, typename TT>
		static auto test(int)
			-> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type()) {}

		template<typename, typename>
		static auto test(...)->std::false_type {}

	public:
		static const bool value = decltype(test<S, T>(0))::value;
	};
}

namespace clparser
{
	template <class ValueType>
	concept IsStreammable = is_streamable<std::stringstream, ValueType>::value;

	template <typename ArgumentType>
		requires IsStreammable<ArgumentType>
	class Argument : public clparser::IArgument
	{
	protected:
		Argument(clparser::ICommandLineOptions& options, std::optional<ArgumentType> defaultValue) :
			storage(),
			defaultValue(defaultValue),
			initialized(false),
			index(options.getArguments().size())
		{
		}

		virtual void internalMatch(const std::vector<std::string>& arguments) = 0;

	public:
		void match(const std::vector<std::string>& arguments) override
		{
			initialized = false;

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
			if (!initialized)
			{
				throw std::logic_error("Uninitialized successfully matched field: setStorage() must be called when a field is sucecssfully matched");
			}
		}

		ArgumentType operator()() const
		{
			if (!initialized)
			{
				throw std::logic_error("Unanitialized field");
			}

			return storage;
		}

	protected:
		void setStorage(const ArgumentType& value)
		{
			storage = value;
			initialized = true;
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
		Argument<ArgumentType>(const Argument<ArgumentType>&) = delete;
		Argument<ArgumentType>& operator= (const Argument<ArgumentType>&) = delete;

	private:
		bool initialized;
		ArgumentType storage;
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