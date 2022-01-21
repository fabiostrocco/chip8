#include "Timer.hpp"

void chip8::Timer::setValue(const uint8_t value)
{
    counter = value;
}

uint8_t chip8::Timer::getValue() const
{
    return counter;
}

void chip8::Timer::updateValue()
{
    auto millisecondsSinceLatestDecrement =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - latestCounterDecrement).count();

    // Timer clock tick
    if (millisecondsSinceLatestDecrement >= TickPeriod)
    {
        if (counter > 0)
        {
            counter--;
        }

        latestCounterDecrement = std::chrono::system_clock::now();
    }
}