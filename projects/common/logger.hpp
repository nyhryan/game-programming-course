#pragma once

#include <spdlog/spdlog.h>
#include <memory>

namespace GameProgramming
{

class Logger
{
public:
    static void init();
    [[nodiscard]] static auto &GetLogger() { return s_Logger; }
private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

} // namespace GameProgramming

#define LOG_INIT() ::GameProgramming::Logger::init()

#define LOG_CRITICAL(fmt, ...) ::GameProgramming::Logger::GetLogger()->critical(fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)    ::GameProgramming::Logger::GetLogger()->debug(fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(fmt, ...)    ::GameProgramming::Logger::GetLogger()->error(fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(fmt, ...)     ::GameProgramming::Logger::GetLogger()->info(fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARN(fmt, ...)     ::GameProgramming::Logger::GetLogger()->warn(fmt __VA_OPT__(, ) __VA_ARGS__)
#define LOG_TRACE(fmt, ...)    ::GameProgramming::Logger::GetLogger()->trace(fmt __VA_OPT__(, ) __VA_ARGS__)