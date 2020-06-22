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

#include "config.h"

#include <fstream>
#include <iostream>

#include "logging.h"

namespace chico {

Config::Config()
    :   basic_rom_(nullptr),
        kernal_rom_(nullptr),
        char_rom_(nullptr) {}

Config::~Config() {
    delete [] char_rom_;
    delete [] kernal_rom_;
    delete [] basic_rom_;
}

void Config::Load() {
    basic_rom_ = LoadImage("c64_roms/basic.rom", 8192);
    kernal_rom_ = LoadImage("c64_roms/kernal.rom", 8192);
    char_rom_ = LoadImage("c64_roms/char.rom", 4096);
    total_lines_ = 312;
    visible_lines_ = 284;
    cycle_per_line_ = 63;
    visible_pixels_ = 403;
    screen_magnification_ = 2;
    cpu_clock_ = 985248;
    fps_ = cpu_clock_ / (total_lines_ * cycle_per_line_);
}

const uint8_t* Config::LoadImage(const char* file_name, int size) {
    std::ifstream is(file_name, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    if (is.fail()) {
        Log(Fatal) << " can't open file: " << file_name;
    }
    const int file_size = is.tellg();
    if (file_size != size) {
        is.close();
        Log(Fatal) << " illegal size " << file_size << " for file: " << file_name;
    }
    uint8_t* buffer = new uint8_t[size];
    is.seekg(0, is.beg);
    is.read((char*)buffer, size);
    is.close();
    return buffer;
}

} // namespace chico
