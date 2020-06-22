// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

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
