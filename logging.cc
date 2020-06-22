// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include <iostream>
#include "logging.h"

namespace chico {

const Logger::LogLevel Logger::kLogLevel = Logger::kInfo;

static const char* kLogLevelNames[] = {
    "info",
    "warning",
    "error",
    "fatal"
};

Logger::Logger(LogLevel level, const char *file, int line)
    :   level_(level),
        file_(file),
        line_(line) {}

Logger::~Logger() {
    if (level_ >= kLogLevel) {
        std::cerr << file_ << ":" << line_ << ": " << kLogLevelNames[level_] << ": ";
        std::cerr << stream_.str() << std::endl;
    }
    if (level_ == kFatal) {
        exit(-1);
    }
}

}  // namespace chico
