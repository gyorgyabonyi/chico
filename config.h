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
