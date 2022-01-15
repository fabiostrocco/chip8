#include "Logger.hpp"

#include <SDL_log.h>
#include <map>
#include <stdarg.h>
#include <stdio.h>

namespace
{
    std::map<logging::Severity, SDL_LogPriority> Chip8ToSDLLogSeverityMap{{logging::Severity::Error, SDL_LOG_PRIORITY_ERROR},
                                                                          {logging::Severity::Warning, SDL_LOG_PRIORITY_WARN},
                                                                          {logging::Severity::Info, SDL_LOG_PRIORITY_INFO},
                                                                          {logging::Severity::Debug, SDL_LOG_PRIORITY_DEBUG}};
}

chip8::Logger::Logger()
    : logging::Logger(logging::Severity::Error)
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
}

void chip8::Logger::setVerbose(const bool verbose)
{
    if (verbose)
    {
        setSeverityLevel(logging::Severity::Debug);
    }
    else
    {
        setSeverityLevel(logging::Severity::Info);
    }
}

void chip8::Logger::logInternal(const logging::Severity severity, const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, Chip8ToSDLLogSeverityMap[severity], format, args);
    va_end(args);
}