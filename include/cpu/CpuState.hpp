#pragma once

namespace chip8
{
	enum class CpuState
    {
        Running,
        WaitForDraw,
        WaitForKey
    };
}