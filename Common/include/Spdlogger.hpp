#pragma once
#include "Configure.hpp"
#include "IConfigure.hpp"
#include "ILogger.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace MEngine
{
class Spdlogger final : public ILogger
{
  private:
    std::shared_ptr<IConfigure> mConfigure = nullptr;

  private:
    virtual void Log(LogLevel level, const std::source_location &loc, const std::string &message) override;
    spdlog::sink_ptr mConsoleSink;
    spdlog::sink_ptr mFileSink;
    std::shared_ptr<spdlog::logger> mLogger;

  public:
    Spdlogger();
    void SetLevel(LogLevel level) override;
};
} // namespace MEngine