#pragma once

namespace chip8
{
    enum ExitCode
    {
        Success = 0,
        CommandLineArgsParseError = 1,
        WindowInitializationFailure = 2
    };
}
