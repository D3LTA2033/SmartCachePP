#pragma once
#include <string>
#include <ostream>

namespace SmartCachePP {

class Logger {
public:
    enum class LogLevel { Info, Error, Debug };
    Logger(std::ostream& output = std::cout, LogLevel level = LogLevel::Info);
    void logInfo(const std::string& message);
    void logError(const std::string& message);
    void logDebug(const std::string& message);

private:
    LogLevel logLevel;
    std::ostream& out;
};

}