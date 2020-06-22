// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_LOGGING_H
#define CHICO_LOGGING_H

#include <sstream>

namespace chico {

class Logger final {
public:
    enum LogLevel {
        kInfo = 0,
        kWarning = 1,
        kError = 2,
        kFatal = 3
    };

    Logger(LogLevel level, const char* file, int line);
    ~Logger();

    constexpr std::ostream& GetStream() { return stream_; }

private:
    static const LogLevel kLogLevel;

    const LogLevel level_;
    const char* file_;
    const int line_;
    std::ostringstream stream_;
};

#define Log(level) ::chico::Logger(::chico::Logger::k##level, __FILE__, __LINE__).GetStream()

}  // namespace chico

#endif  // CHICO_LOGGING_H
