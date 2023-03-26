#include "Logger.h"

#include <spdlog/spdlog.h>

#include <cstdio>

void Logger::rayLog(int msgType, const char* text, va_list args)
{
    char message[1024];
    vsprintf_s(message, text, args);

    switch (msgType)
    {
    case 3:
        spdlog::info(message);
        break;
    case 5:
        spdlog::error(message);
        break;
    case 4:
        spdlog::warn(message);
        break;
    case 2:
        spdlog::debug(message);
        break;
    }
}

void Logger::setDebug(bool debug)
{
    spdlog::set_level(debug ? spdlog::level::debug : spdlog::level::info);
}

void Logger::info(const std::string& message)
{
    spdlog::info(message);
}

void Logger::error(const std::string& message)
{
    spdlog::error(message);
}

void Logger::warn(const std::string& message)
{
    spdlog::warn(message);
}

void Logger::debug(const std::string& message)
{
    spdlog::debug(message);
}
