#pragma once

namespace chip8
{
    enum ExitCode
    {
        Success = 0,
        CommandLineArgsParseError = 1,
        WindowInitializationFailure = 2,
        AudioInitializationFailure = 3,
        RomLoadFailure = 4,
        CpuError = 5
    };
}
