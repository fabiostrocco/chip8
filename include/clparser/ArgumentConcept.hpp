#pragma once

#include <sstream>
#include <type_traits>

namespace
{
    template <typename StreamType, typename ArgumentType>
    class is_streamable
    {
        template <typename StreamType1, typename ArgumentType1>
        static constexpr auto test(int) -> decltype(std::declval<StreamType1&>() << std::declval<ArgumentType1>(), bool())
        {
            return true;
        }

        template <typename, typename>
        static constexpr auto test(...) -> bool
        {
            return false;
        }

      public:
        static constexpr bool value = test<StreamType, ArgumentType>(0);
    };
}

namespace clparser
{
    template <class ValueType>
    concept ArgumentConcept = is_streamable<std::stringstream, ValueType>::value && !std::is_reference<ValueType>::value && !std::is_pointer<ValueType>::value;
}
