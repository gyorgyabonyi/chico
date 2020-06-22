/*
Copyright (c) 2020 Gyorgy Abonyi.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
