#pragma once

#include <logging/Logger.hpp>

namespace chip8
{
    class Logger : public logging::Logger
    {
      public:
        Logger();
        void setVerbose(const bool verbose);

      protected:
        void logInternal(const logging::Severity severity, const char* ch, ...) const override;

      private:
        // Logger should alway passed by reference across the application.
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger) = delete;
    };
}