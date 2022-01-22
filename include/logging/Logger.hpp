#pragma once

#include <cstdarg>
#include <iostream>

#include "LoggerConcept.hpp"
#include "Severity.hpp"

namespace
{
    template <typename T>
    auto toPrintfFormat(T&& x)
    {
        if constexpr (logging::is_string<T>::value)
        {
            return x.c_str();
        }
        else
        {
            return x;
        }
    }
}

namespace logging
{
    class Logger
    {
      public:
        template <logging::LoggerConcept... Args>
        void log(const Severity severity, const std::string& format, Args&&... args) const
        {
            if (severity > severityLevel)
            {
                return;
            }

            logInternal(severity, format.c_str(), toPrintfFormat(std::forward<Args>(args))...);
        }

        template <logging::LoggerConcept... Args>
        void logError(const std::string& format, Args&&... args) const
        {
            log(logging::Severity::Error, format, std::forward<Args>(args)...);
        }

        template <logging::LoggerConcept... Args>
        void logWarning(const std::string& format, Args&&... args) const
        {
            log(logging::Severity::Warning, format, std::forward<Args>(args)...);
        }

        template <logging::LoggerConcept... Args>
        void logInfo(const std::string& format, Args&&... args) const
        {
            log(logging::Severity::Info, format, std::forward<Args>(args)...);
        }

        template <logging::LoggerConcept... Args>
        void logDebug(const std::string& format, Args&&... args) const
        {
            log(logging::Severity::Debug, format, std::forward<Args>(args)...);
        }

        void setSeverityLevel(const Severity level)
        {
            severityLevel = level;
        }

      protected:
        Logger(Severity severityLevel)
            : severityLevel(severityLevel)
        {
        }

        virtual void logInternal(const Severity severity, const char* ch, ...) const = 0;

      private:
        Severity severityLevel;
    };
}