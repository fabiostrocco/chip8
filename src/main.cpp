//#include <SDL.h>

#include "CommandLineOptions.hpp"
#include "ExitCode.hpp"
#include "Logger.hpp"
#include "clparser/ArgumentFormatException.hpp"
#include "clparser/ArgumentNotFoundException.hpp"
#include "clparser/CommandLineParser.hpp"
#include "emulator/Emulator.hpp"
#include "logging/Severity.hpp"
#include "metadata.hpp"

int main(int argc, char* argv[])
{
    chip8::CommandLineOptions options;
    clparser::CommandLineParser parser(argc, argv);
    chip8::Logger logger;

    try
    {
        parser.parse(options);
        if (options.version())
        {
            std::cout << chip8::metadata::ProgramName << " version " << chip8::metadata::Version << std::endl;
            return chip8::ExitCode::Success;
        }

        if (options.help())
        {
            std::cout << options.getHelpMessage(chip8::metadata::ProgramName) << std::endl;
            return chip8::ExitCode::Success;
        }

        logger.setVerbose(options.verbose());

        chip8::Emulator emulator(logger, options.romName());
        emulator.run();
    }
    catch (clparser::ArgumentNotFoundException& argNotFound)
    {
        logger.logError(argNotFound.what());
        std::cout << options.getHelpMessage(chip8::metadata::ProgramName) << std::endl;
        return chip8::ExitCode::CommandLineArgsParseError;
    }
    catch (clparser::ArgumentFormatException& argWrongFormat)
    {
        logger.logError(argWrongFormat.what());
        std::cout << options.getHelpMessage(chip8::metadata::ProgramName) << std::endl;
        return chip8::ExitCode::CommandLineArgsParseError;
    }

    return 0;
}
