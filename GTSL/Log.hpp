#pragma once

#include <chrono>
#include <format>
#include <string>

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

    class Log final
    {
    public:
        Log() = default;
    	~Log() = default;

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

    private:
        struct LogEntry
        {
            LogEntry(LogLevel level, std::string message)
                : m_level(level), m_message(std::move(message)), m_timestamp(std::chrono::system_clock::now())
            {

            }

            LogLevel level() const
            {
                return m_level;
            }

            const char* message() const
            {
                return m_message.c_str();
            }

            const std::chrono::system_clock::time_point& timestamp() const
            {
                return m_timestamp;
            }

        private:
            LogLevel m_level;
            std::string m_message;
            std::chrono::system_clock::time_point m_timestamp;
        };

    	void logEntry(const LogEntry& entry)
        {

            std::string timeStr = std::format("{:%Y-%m-%d %H:%M:%S}", entry.timestamp());
            timeStr.pop_back();
            std::cout << "[" << timeStr << "] " << ToString(entry.level()) << ": " << entry.message() << std::endl;
        }

        void LogMessage(LogLevel level, const char* message)
        {
            LogEntry entry(level, message);
            logEntry(entry);
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
    GTSL::Log().Trace(std::format(fmt,__VA_ARGS__));

#ifdef _DEBUG
#define LOG_DEBUG(fmt,...)\
    GTSL::Log().Debug(std::format(fmt,__VA_ARGS__));
#else
#define LOG_DEBUG(fmt,...)
#endif

#define LOG_INFO(fmt,...)\
    GTSL::Log().Info(std::format(fmt,__VA_ARGS__));

#define LOG_WARN(fmt,...)\
    GTSL::Log().Warn(std::format(fmt,__VA_ARGS__));

#define LOG_ERROR(fmt,...)\
    GTSL::Log().Error(std::format(fmt,__VA_ARGS__));

#define LOG_FATAL(fmt,...)\
    GTSL::Log().Fatal(std::format(fmt,__VA_ARGS__));