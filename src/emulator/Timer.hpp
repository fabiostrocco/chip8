#pragma once

#include <chrono>
#include <cstdint>

#include "ITimer.hpp"

namespace chip8
{
    class Timer : public ITimer
    {
      public:
        void setValue(const uint8_t value) override;
        uint8_t getValue() const override;
        void updateValue() override;

      private:
        static constexpr uint64_t TickFrequency = 60;
        static constexpr uint64_t TickPeriod = static_cast<uint64_t>(1.0 / static_cast<double>(TickFrequency) * 1000);

        std::chrono::time_point<std::chrono::system_clock> latestCounterDecrement = std::chrono::system_clock::now();
        uint8_t counter = 0;
    };
}