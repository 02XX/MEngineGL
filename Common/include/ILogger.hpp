#pragma once
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <source_location>
#include <string>

namespace MEngine
{
enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};
class ILogger
{
  public:
    virtual ~ILogger() = default;
    template <typename... Args>
    void Trace(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Trace, loc, message);
    }
    template <typename... Args>
    void Debug(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Debug, loc, message);
    }
    template <typename... Args>
    void Warn(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Warn, loc, message);
    }
    template <typename... Args>
    void Info(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Info, loc, message);
    }
    template <typename... Args>
    void Error(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Error, loc, message);
    }
    template <typename... Args>
    void Fatal(const std::source_location &loc, fmt::format_string<Args...> format, Args &&...args)
    {
        std::string message = fmt::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Fatal, loc, message);
    }
    virtual void SetLevel(LogLevel level) = 0;

  private:
    virtual void Log(LogLevel level, const std::source_location &loc, const std::string &message) = 0;
};

class LoggerConfigure
{
  public:
    std::string LogFilePath = "logs/log.txt";
    LogLevel LogLevel = LogLevel::Trace;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::LoggerConfigure>
{
    static void to_json(json &j, const MEngine::LoggerConfigure &m)
    {
        j = {
            {"Logger", {{"LogFilePath", m.LogFilePath}, {"LogLevel", std::string(magic_enum::enum_name(m.LogLevel))}}}};
    }
    static void from_json(const json &j, MEngine::LoggerConfigure &m)
    {
        const auto &logger = j.at("Logger");
        m.LogFilePath = logger.at("LogFilePath").get<std::string>();
        auto level = logger.at("LogLevel").get<std::string>();
        m.LogLevel = magic_enum::enum_cast<MEngine::LogLevel>(level).value_or(MEngine::LogLevel::Info);
    }
};
} // namespace nlohmann
