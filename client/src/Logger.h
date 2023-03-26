#pragma once

#include <cstdarg>
#include <string>

class Logger
{
public:
    static void rayLog(int msgType, const char* text, va_list args);
    static void setDebug(bool debug);
    static void info(const std::string& message);
    static void error(const std::string& message);
    static void warn(const std::string& message);
    static void debug(const std::string& message);
};
