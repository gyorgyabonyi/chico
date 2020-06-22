// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_CONFIG_H
#define CHICO_CONFIG_H

#include <cstdint>

namespace chico {

class Config final {
public:
    Config();
    ~Config();

    constexpr const uint8_t* GetBasicRom() const { return basic_rom_; }
    constexpr const uint8_t* GetKernalRom() const { return kernal_rom_; }
    constexpr const uint8_t* GetCharRom() const { return char_rom_; }
    constexpr int GetTotalLines() const { return total_lines_; };
    constexpr int GetVisibleLines() const { return visible_lines_; };
    constexpr int GetCyclesPerLine() const { return cycle_per_line_; };
    constexpr int GetVisiblePixels() const { return visible_pixels_; };
    constexpr int GetScreenMagnification() const { return screen_magnification_; }
    constexpr int GetCpuClock() const { return cpu_clock_; }
    constexpr int GetFps() const { return fps_; }

    void Load();

private:
    const uint8_t* basic_rom_;
    const uint8_t* kernal_rom_;
    const uint8_t* char_rom_;
    int total_lines_;
    int visible_lines_;
    int cycle_per_line_;
    int visible_pixels_;
    int screen_magnification_;
    int cpu_clock_;
    int fps_;

    const uint8_t* LoadImage(const char* file_name, int size);
};

} // namespace chico

#endif  // CHICO_CONFIG_H
