#pragma once

#include <type_traits>

namespace logging
{

    template <typename T>
    using is_string = std::is_same<typename std::decay<T>::type, std::string>;

    template <typename T>
    using is_const_char_ptr = std::is_same<typename std::decay<T>::type, const char*>;

    template <typename T>
    using is_char_ptr = std::is_same<typename std::decay<T>::type, char*>;

    template <typename LogArgument>
    concept LoggerConcept = std::is_arithmetic<typename std::decay<LogArgument>::type>::value || logging::is_string<LogArgument>::value || is_char_ptr<LogArgument>::value ||
        is_const_char_ptr<LogArgument>::value || std::is_enum<typename std::decay<LogArgument>::type>::value;
}