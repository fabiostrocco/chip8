#pragma once

#include <cstdint>

namespace chip8::binding
{
    using MatchingPatternType = uint8_t;
    static constexpr MatchingPatternType placeholder = 255u;

    template <MatchingPatternType unit>
    concept BindingUnitTypeConcept = (unit <= 0xf || unit == placeholder);

    static constexpr inline uint8_t isPlaceholder(const MatchingPatternType& unit)
    {
        return unit == placeholder ? 1 : 0;
    }

    static constexpr inline uint8_t
    numberOfPlaceHolders(MatchingPatternType unit1, MatchingPatternType unit2, MatchingPatternType unit3, MatchingPatternType unit4)
    {
        return isPlaceholder(unit1) + isPlaceholder(unit2) + isPlaceholder(unit3) + isPlaceholder(unit4);
    }
}