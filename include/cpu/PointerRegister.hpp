#pragma once

#include <functional>
#include <type_traits>

#include "CpuExecutionException.hpp"

namespace chip8
{
    template <typename RegisterType>
    concept PointerRegisterConcept = std::is_unsigned<RegisterType>::value;

    template <PointerRegisterConcept RegisterType, CpuErrorCode underflowErrorCode, CpuErrorCode overflowErrorCode>
    class PointerRegister
    {
      public:
        PointerRegister(std::function<void(RegisterType)> writeValidator, std::function<void(RegisterType)> readValidator)
            : writeValidator(writeValidator)
            , readValidator(readValidator)
            , pointer(0)
        {
        }

        RegisterType operator*() const
        {
            return pointer;
        }

        RegisterType operator+(const RegisterType& value) const
        {
            RegisterType postIncrementValue = pointer + value;

            // If overflow

            if (postIncrementValue < pointer)
            {
                throw CpuExecutionException(overflowErrorCode);
            }

            readValidator(postIncrementValue);

            return postIncrementValue;
        }

        RegisterType operator-(const RegisterType& value) const
        {
            RegisterType postDecrementValue = pointer - value;

            // If underflow

            if (postDecrementValue > pointer)
            {
                throw CpuExecutionException(underflowErrorCode);
            }

            readValidator(postDecrementValue);

            return postDecrementValue;
        }

        PointerRegister& operator=(const RegisterType value)
        {
            writeValidator(value);
            pointer = value;
            return *this;
        }

        PointerRegister& operator+=(const RegisterType value)
        {
            RegisterType postIncrementValue = pointer + value;

            // If overflow

            if (postIncrementValue < pointer)
            {
                throw CpuExecutionException(overflowErrorCode);
            }

            writeValidator(postIncrementValue);

            pointer = postIncrementValue;

            return *this;
        }

        PointerRegister& operator-=(const RegisterType value)
        {
            RegisterType postDecrementValue = pointer - value;

            // If underflow

            if (postDecrementValue > pointer)
            {
                throw CpuExecutionException(overflowErrorCode);
            }

            writeValidator(postDecrementValue);

            pointer = postDecrementValue;

            return *this;
        }

      private:
        RegisterType pointer;
        std::function<void(RegisterType)> writeValidator;
        std::function<void(RegisterType)> readValidator;
    };
}