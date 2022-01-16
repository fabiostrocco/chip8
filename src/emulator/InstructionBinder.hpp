#pragma once

#include <functional>
#include <optional>
#include <stdint.h>

namespace chip8
{
    using BindingUnitType = uint8_t;
    static constexpr uint8_t placeholder = 255;

    static constexpr inline int isPlaceholder(const BindingUnitType& unit)
    {
        return unit == placeholder ? 1 : 0;
    }

    static constexpr inline int
    numberOfPlaceHolders(BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4)
    {
        return isPlaceholder(unit1) + isPlaceholder(unit2) + isPlaceholder(unit3) + isPlaceholder(unit4);
    }

    class IBinder
    {
      public:
        virtual void match(const uint8_t byte1, const uint8_t byte2) = 0;
    };

    template <BindingUnitType unit>
    concept BindingUnitTypeConcept = unit <= 0xf || unit == placeholder;

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    requires(BindingUnitTypeConcept<unit1>&& BindingUnitTypeConcept<unit2>&& BindingUnitTypeConcept<unit3>&&
                 BindingUnitTypeConcept<unit4>) class InstructionBinderBase : public IBinder
    {
      public:
        void match(const uint8_t byte1, const uint8_t byte2) override
        {
            BindingUnitType value1 = (byte1 & 0xF0) >> 4;
            BindingUnitType value2 = byte1 & 0x0F;
            BindingUnitType value3 = (byte2 & 0xF0) >> 4;
            BindingUnitType value4 = byte2 & 0x0F;

            if (internalMatch(value1, value2, value3, value4))
            {
                onMatch(value1, value2, value3, value4);
            }
        }

      protected:
        std::vector<BindingUnitType> bindArguments(const BindingUnitType value1,
                                                   const BindingUnitType value2,
                                                   const BindingUnitType value3,
                                                   const BindingUnitType value4)
        {
            std::vector<BindingUnitType> args;
            addIfPlaceHolder(args, unit1, value1);
            addIfPlaceHolder(args, unit2, value2);
            addIfPlaceHolder(args, unit3, value3);
            addIfPlaceHolder(args, unit4, value4);

            return args;
        }

        virtual void onMatch(const BindingUnitType value1,
                             const BindingUnitType value2,
                             const BindingUnitType value3,
                             const BindingUnitType value4) = 0;

      private:
        bool internalMatch(const BindingUnitType value1,
                           const BindingUnitType value2,
                           const BindingUnitType value3,
                           const BindingUnitType value4)
        {
            bool match = true;
            match &= matchUnitValue(unit1, value1);
            match &= matchUnitValue(unit2, value2);
            match &= matchUnitValue(unit3, value3);
            match &= matchUnitValue(unit4, value4);

            return match;
        }

        void addIfPlaceHolder(std::vector<BindingUnitType>& list, const BindingUnitType unit, const BindingUnitType value)
        {
            if (unit == placeholder)
            {
                list.push_back(value);
            }
        }

        bool matchUnitValue(const BindingUnitType unit, const BindingUnitType value)
        {
            return value == unit || unit == placeholder;
        }
    };

    template <BindingUnitType unit1,
              BindingUnitType unit2,
              BindingUnitType unit3,
              BindingUnitType unit4,
              size_t NumberOfPlaceHolders>
    class InstructionBinderDerived;

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    class InstructionBinderDerived<unit1, unit2, unit3, unit4, 0> : public InstructionBinderBase<unit1, unit2, unit3, unit4>
    {
      public:
        InstructionBinderDerived(std::function<void()> f)
        {
            matchCallback = f;
        }

      protected:
        void onMatch(const BindingUnitType value1,
                     const BindingUnitType value2,
                     const BindingUnitType value3,
                     const BindingUnitType value4) override
        {
            matchCallback();
        }

      private:
        std::function<void()> matchCallback;
    };

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    class InstructionBinderDerived<unit1, unit2, unit3, unit4, 1> : public InstructionBinderBase<unit1, unit2, unit3, unit4>
    {
      public:
        InstructionBinderDerived(std::function<void(BindingUnitType)> f)
        {
            matchCallback = f;
        }

      protected:
        void onMatch(const BindingUnitType value1,
                     const BindingUnitType value2,
                     const BindingUnitType value3,
                     const BindingUnitType value4) override
        {
            std::vector<BindingUnitType> args =
                chip8::InstructionBinderBase<unit1, unit2, unit3, unit4>::bindArguments(value1, value2, value3, value4);
            matchCallback(args[0]);
        }

      private:
        std::function<void(BindingUnitType)> matchCallback;
    };

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    class InstructionBinderDerived<unit1, unit2, unit3, unit4, 2> : public InstructionBinderBase<unit1, unit2, unit3, unit4>
    {
      public:
        InstructionBinderDerived(std::function<void(BindingUnitType, BindingUnitType)> f)
        {
            matchCallback = f;
        }

      protected:
        void onMatch(const BindingUnitType value1,
                     const BindingUnitType value2,
                     const BindingUnitType value3,
                     const BindingUnitType value4) override
        {
            std::vector<BindingUnitType> args =
                chip8::InstructionBinderBase<unit1, unit2, unit3, unit4>::bindArguments(value1, value2, value3, value4);
            matchCallback(args[0], args[1]);
        }

      private:
        std::function<void(BindingUnitType, BindingUnitType)> matchCallback;
    };

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    class InstructionBinderDerived<unit1, unit2, unit3, unit4, 3> : public InstructionBinderBase<unit1, unit2, unit3, unit4>
    {
      public:
        InstructionBinderDerived(std::function<void(BindingUnitType, BindingUnitType, BindingUnitType)> f)
        {
            matchCallback = f;
        }

      protected:
        void onMatch(const BindingUnitType value1,
                     const BindingUnitType value2,
                     const BindingUnitType value3,
                     const BindingUnitType value4) override
        {
            std::vector<BindingUnitType> args =
                chip8::InstructionBinderBase<unit1, unit2, unit3, unit4>::bindArguments(value1, value2, value3, value4);
            matchCallback(args[0], args[1], args[2]);
        }

      private:
        std::function<void(BindingUnitType, BindingUnitType, BindingUnitType)> matchCallback;
    };

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    class InstructionBinderDerived<unit1, unit2, unit3, unit4, 4> : public InstructionBinderBase<unit1, unit2, unit3, unit4>
    {
      public:
        InstructionBinderDerived(std::function<void(BindingUnitType, BindingUnitType, BindingUnitType, BindingUnitType)> f)
        {
            matchCallback = f;
        }

      protected:
        void onMatch(const BindingUnitType value1,
                     const BindingUnitType value2,
                     const BindingUnitType value3,
                     const BindingUnitType value4) override
        {
            std::vector<BindingUnitType> args =
                chip8::InstructionBinderBase<unit1, unit2, unit3, unit4>::bindArguments(value1, value2, value3, value4);
            matchCallback(args[0], args[1], args[2], args[3]);
        }

      private:
        std::function<void(BindingUnitType, BindingUnitType, BindingUnitType, BindingUnitType)> matchCallback;
    };

    template <BindingUnitType unit1, BindingUnitType unit2, BindingUnitType unit3, BindingUnitType unit4>
    using InstructionBinder =
        InstructionBinderDerived<unit1, unit2, unit3, unit4, numberOfPlaceHolders(unit1, unit2, unit3, unit4)>;
}
