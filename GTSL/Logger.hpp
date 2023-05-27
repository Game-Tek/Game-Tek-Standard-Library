#pragma once

#include <chrono>
#include "String/StringCommon.hpp"
#include "Format.hpp"
namespace GTSL
{
	enum class LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL
    };

    class Logger
    {
    public:
        Logger() = default;
    	~Logger() = default;

        void Trace(const char* message)
        {
            LogMessage(LogLevel::TRACE, message);
        }

        void Debug(const char* message)
        {
#if _DEBUG
            LogMessage(LogLevel::DEBUG, message);
#endif
        }

        void Info(const char* message)
        {
            LogMessage(LogLevel::INFO, message);
        }

        void Warn(const char* message)
        {
            LogMessage(LogLevel::WARN, message);
        }

        void Error(const char* message)
        {
            LogMessage(LogLevel::ERR, message);
        }

        void Fatal(const char* message)
        {
            LogMessage(LogLevel::FATAL, message);
        }

        void Trace(const std::string& message)
        {
            Trace(message.c_str());
        }

        void Debug(const std::string& message)
        {
            Debug(message.c_str());
        }

        void Info(const std::string& message)
        {
            Info(message.c_str());
        }

        void Warn(const std::string& message)
        {
            Warn(message.c_str());
        }

        void Error(const std::string& message)
        {
            Error(message.c_str());
        }

        void Fatal(const std::string& message)
        {
            Fatal(message.c_str());
        }

    protected:

        virtual void OnLog(LogLevel level, std::string message) {}

    private:
        void LogMessage(LogLevel level, const char* message)
        {
            std::string timeStr = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());
            timeStr.pop_back();

            const auto m = GTSL::FormatString("[{}] {}: {}",timeStr,ToString(level),message);
            std::cout << m << std::endl;
            OnLog(level, m);
        }

        static const char* ToString(LogLevel level)
        {
            switch (level)
            {
            case LogLevel::TRACE:
                return "TRACE";
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARN:
                return "WARN";
            case LogLevel::ERR:
                return "ERROR";
            case LogLevel::FATAL:
                return "FATAL";
            default:
                return "UNKNOWN";
            }
        }
    };
}

#define LOG_TRACE(fmt,...)\
    GTSL::Logger().Trace(std::format(fmt,__VA_ARGS__));

#ifdef _DEBUG
#define LOG_DEBUG(fmt,...)\
    GTSL::Logger().Debug(std::format(fmt,__VA_ARGS__));
#else
#define LOG_DEBUG(fmt,...)
#endif

#define LOG_INFO(fmt,...)\
    GTSL::Logger().Info(std::format(fmt,__VA_ARGS__));

#define LOG_WARN(fmt,...)\
    GTSL::Logger().Warn(std::format(fmt,__VA_ARGS__));

#define LOG_ERROR(fmt,...)\
    GTSL::Logger().Error(std::format(fmt,__VA_ARGS__));

#define LOG_FATAL(fmt,...)\
    GTSL::Logger().Fatal(std::format(fmt,__VA_ARGS__));