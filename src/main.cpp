//#include <SDL.h>

#include <clparser/ArgumentFormatException.hpp>
#include <clparser/ArgumentNotFoundException.hpp>
#include <clparser/CommandLineParser.hpp>
#include <cpu/CpuExecutionException.hpp>
#include <cpu/RomLoadFailureException.hpp>
#include <emulator/AudioInitializationException.hpp>
#include <emulator/Emulator.hpp>
#include <emulator/WindowInitializationException.hpp>
#include <logging/Severity.hpp>

#include "CommandLineOptions.hpp"
#include "ExitCode.hpp"
#include "Logger.hpp"
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
        emulator.run(chip8::metadata::ProgramName, chip8::metadata::Version, options.clock());
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
    catch (chip8::WindowInitializationException& windowFailure)
    {
        logger.logError(windowFailure.what());
        return chip8::ExitCode::WindowInitializationFailure;
    }
    catch (chip8::AudioInitializationException& audioFailure)
    {
        logger.logError(audioFailure.what());
        return chip8::ExitCode::AudioInitializationFailure;
    }
    catch (chip8::RomLoadFailureException& romLoadFailure)
    {
        logger.logError(romLoadFailure.what());
        return chip8::ExitCode::RomLoadFailure;
    }
    catch (chip8::CpuExecutionException& cpuError)
    {
        logger.logError(cpuError.what());
        return chip8::ExitCode::CpuError;
    }

    return chip8::ExitCode::Success;
}
