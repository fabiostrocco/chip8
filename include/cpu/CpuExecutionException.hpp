#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace chip8
{
    enum class CpuErrorCode
    {
        StackOverflow,
        StackUnderflow,
        AddressOutOfBound,
        UnmatchedInstruction,
        BadKeyboardIndex,
        UnsupportedSysInstruction
    };

    class CpuExecutionException : public std::runtime_error
    {
      public:
        explicit CpuExecutionException(const CpuErrorCode errorCode)
            : std::runtime_error(errorCodeToString(errorCode))
        {
        }

        // template <typename NumericType>
        // static std::string toHex(NumericType number)
        // {
        //     std::stringstream stream;
        //     stream << "0x";
        //     stream << std::hex << number;
        //     std::string result(stream.str());
        //     return result;
        // }

      private:
        std::string errorCodeToString(const chip8::CpuErrorCode errorCode)
        {
            switch (errorCode)
            {
            case chip8::CpuErrorCode::StackOverflow:
                return "stack overflow";
            case chip8::CpuErrorCode::StackUnderflow:
                return "stack underflow";
            case chip8::CpuErrorCode::AddressOutOfBound:
                return "address out of bound";
            case chip8::CpuErrorCode::UnmatchedInstruction:
                return "invalid instruction";
            case chip8::CpuErrorCode::BadKeyboardIndex:
                return "keyboard index does not identify a key";
            case chip8::CpuErrorCode::UnsupportedSysInstruction:
                return "Unsupported legacy SYS instruction";
            default:
                throw std::logic_error("Unhandled CpuErrorCode");
            }
        }
    };
}