#include "logger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace GameProgramming
{
std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::init()
{
    spdlog::set_pattern("[%T] %^[%L]%$: %v");
    s_Logger = spdlog::stdout_color_mt("Logger");
    s_Logger->set_level(spdlog::level::trace);
}

} // namespace GameProgramming