#pragma once

#include <functional>
#include <optional>
#include <stdint.h>
#include <tuple>
#include <utility>
#include <vector>

#include "IBinder.hpp"
#include "InstructionBinding.hpp"

namespace chip8
{
    template <binding::MatchingPatternType pattern1,
              binding::MatchingPatternType pattern2,
              binding::MatchingPatternType pattern3,
              binding::MatchingPatternType pattern4,
              typename... T>
    requires(binding::BindingUnitTypeConcept<pattern1>&& binding::BindingUnitTypeConcept<pattern2>&& binding::BindingUnitTypeConcept<pattern3>&&
                 binding::BindingUnitTypeConcept<pattern4>) class _InstructionBinder : public IBinder
    {
      public:
        _InstructionBinder(std::function<void(T...)> matchCallback)
            : matchCallback(matchCallback)
        {
        }

        bool match(const uint8_t byte1, const uint8_t byte2) override
        {
            return match(byte1, byte2, std::make_index_sequence<sizeof...(T)>());
        }

      private:
        template <size_t... valueIndex>
        bool match(const uint8_t byte1, const uint8_t byte2, std::index_sequence<valueIndex...>)
        {
            std::vector<binding::MatchingPatternType> boundValues;
            bool hasMatch = true;
            hasMatch &= internalMatch((byte1 & 0xF0) >> 4, pattern1, boundValues);
            hasMatch &= internalMatch(byte1 & 0x0F, pattern2, boundValues);
            hasMatch &= internalMatch((byte2 & 0xF0) >> 4, pattern3, boundValues);
            hasMatch &= internalMatch(byte2 & 0x0F, pattern4, boundValues);

            if (hasMatch)
            {
                matchCallback(boundValues[valueIndex]...);
                return true;
            }

            return false;
        }

        bool internalMatch(const binding::MatchingPatternType value,
                           const binding::MatchingPatternType pattern,
                           std::vector<binding::MatchingPatternType>& boundValues)
        {
            if (binding::isPlaceholder(pattern))
            {
                boundValues.push_back(value);
                return true;
            }

            return value == pattern;
        }

        std::function<void(T...)> matchCallback;
    };

    template <binding::MatchingPatternType pattern1,
              binding::MatchingPatternType pattern2,
              binding::MatchingPatternType pattern3,
              binding::MatchingPatternType pattern4,
              uint8_t n,
              class... T>
    struct _InstructionBinderWrapper
    {
        using type = typename _InstructionBinderWrapper<pattern1, pattern2, pattern3, pattern4, n - 1, binding::MatchingPatternType, T...>::type;
    };

    template <binding::MatchingPatternType pattern1,
              binding::MatchingPatternType pattern2,
              binding::MatchingPatternType pattern3,
              binding::MatchingPatternType pattern4,
              class... T>
    struct _InstructionBinderWrapper<pattern1, pattern2, pattern3, pattern4, 0, T...>
    {
        using type = _InstructionBinder<pattern1, pattern2, pattern3, pattern4, T...>;
    };

    template <binding::MatchingPatternType pattern1,
              binding::MatchingPatternType pattern2,
              binding::MatchingPatternType pattern3,
              binding::MatchingPatternType pattern4>
    using InstructionBinder =
        typename _InstructionBinderWrapper<pattern1, pattern2, pattern3, pattern4, binding::numberOfPlaceHolders(pattern1, pattern2, pattern3, pattern4)>::type;
}
