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

#include "keyboard.h"

#include <SDL2/SDL_scancode.h>

namespace chico {

enum : uint8_t {
    kDelete, kReturn,   kLeftRight, kF7,        kF1,         kF3,        kF5, kUpDown,
    k3,      kW,        kA,         k4,         kZ,          kS,         kE,  kLeftShift,
    k5,      kR,        kD,         k6,         kC,          kF,         kT,  kX,
    k7,      kY,        kG,         k8,         kB,          kH,         kU,  kV,
    k9,      kI,        kJ,         k0,         kM,          kK,         kO,  kN,
    kPlus,   kP,        kL,         kMinus,     kPeriod,     kColon,     kAt, kComma,
    kPound,  kAsterisk, kSemicolon, kClearHome, kRightShift, kEqual,     kUp, kSlash,
    k1,      kLeft,     kControl,   k2,         kSpace,      kCommodore, kQ,  kRunStop,
    kLastKey
};


Keyboard::Keyboard()
    :   columns_(0),
        rows_{0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu} {}

void Keyboard::Reset() {
    columns_ = 0;
    for (int i = 0; i < 8; i++) {
        rows_[i] = 0xffu;
    }
}

void Keyboard::SetColumns(uint8_t columns) {
    columns_ = columns;
}

uint8_t Keyboard::GetRows() {
    uint8_t value = 0xff;
    for (int i = 0; i < 8; i++) {
        if (~columns_ & (1u << i)) {
            value &= rows_[i];
        }
    }
    return value;
}

void Keyboard::OnKeyDown(int scancode) {
    const int c64_key = FindMapping(scancode);
    if (c64_key == kLastKey) {
        return;
    }
    const int row = 1u << (c64_key & 0x7u);
    const int column = c64_key >> 3u;
    rows_[column] &= ~row;
}

void Keyboard::OnKeyUp(int scancode) {
    const int c64_key = FindMapping(scancode);
    if (c64_key == kLastKey) {
        return;
    }
    const int row = 1u << (c64_key & 0x7u);
    const int column = c64_key >> 3u;
    rows_[column] |= row;
}

int Keyboard::FindMapping(int host_key) const {
    for (const Mapping* mapping = kMappings; mapping->c64_key_ != kLastKey; mapping++) {
        if (mapping->host_key_ == host_key) {
            return mapping->c64_key_;
        }
    }
    return kLastKey;
}

/*
POUND
CLEAR/HOME
UP
RUN/STOP
*/


const Keyboard::Mapping Keyboard::kMappings[] = {
    { SDL_SCANCODE_BACKSPACE, kDelete },
    { SDL_SCANCODE_RETURN, kReturn },
    { SDL_SCANCODE_LEFT, kLeftRight },
    { SDL_SCANCODE_RIGHT, kLeftRight },
    { SDL_SCANCODE_F7, kF7 },
    { SDL_SCANCODE_F1, kF1 },
    { SDL_SCANCODE_F3, kF3 },
    { SDL_SCANCODE_F5, kF5 },
    { SDL_SCANCODE_UP, kUpDown },
    { SDL_SCANCODE_DOWN, kUpDown },
    { SDL_SCANCODE_3, k3 },
    { SDL_SCANCODE_W, kW },
    { SDL_SCANCODE_A, kA },
    { SDL_SCANCODE_4, k4 },
    { SDL_SCANCODE_Z, kZ },
    { SDL_SCANCODE_S, kS },
    { SDL_SCANCODE_E, kE },
    { SDL_SCANCODE_LSHIFT, kLeftShift },
    { SDL_SCANCODE_5, k5 },
    { SDL_SCANCODE_R, kR },
    { SDL_SCANCODE_D, kD },
    { SDL_SCANCODE_6, k6 },
    { SDL_SCANCODE_C, kC },
    { SDL_SCANCODE_F, kF },
    { SDL_SCANCODE_T, kT },
    { SDL_SCANCODE_X, kX },
    { SDL_SCANCODE_7, k7 },
    { SDL_SCANCODE_Y, kY },
    { SDL_SCANCODE_G, kG },
    { SDL_SCANCODE_8, k8 },
    { SDL_SCANCODE_B, kB },
    { SDL_SCANCODE_H, kH },
    { SDL_SCANCODE_U, kU },
    { SDL_SCANCODE_V, kV },
    { SDL_SCANCODE_9, k9 },
    { SDL_SCANCODE_I, kI },
    { SDL_SCANCODE_J, kJ },
    { SDL_SCANCODE_0, k0 },
    { SDL_SCANCODE_M, kM },
    { SDL_SCANCODE_K, kK },
    { SDL_SCANCODE_O, kO },
    { SDL_SCANCODE_N, kN },
    { SDL_SCANCODE_MINUS, kPlus },
    { SDL_SCANCODE_P, kP },
    { SDL_SCANCODE_L, kL },
    { SDL_SCANCODE_EQUALS, kMinus },
    { SDL_SCANCODE_PERIOD, kPeriod },
    { SDL_SCANCODE_SEMICOLON, kColon },
    { SDL_SCANCODE_LEFTBRACKET, kAt },
    { SDL_SCANCODE_COMMA, kComma },
    { 0, kPound },
    { SDL_SCANCODE_RIGHTBRACKET, kAsterisk },
    { SDL_SCANCODE_APOSTROPHE, kSemicolon },
    { 0, kClearHome },
    { SDL_SCANCODE_RSHIFT, kRightShift },
    { SDL_SCANCODE_BACKSLASH, kEqual },
    { 0, kUp },
    { SDL_SCANCODE_SLASH, kSlash },
    { SDL_SCANCODE_1, k1 },
    { SDL_SCANCODE_ESCAPE, kLeft },
    { SDL_SCANCODE_TAB, kControl },
    { SDL_SCANCODE_2, k2 },
    { SDL_SCANCODE_SPACE, kSpace },
    { SDL_SCANCODE_COMPUTER, kCommodore },
    { SDL_SCANCODE_Q, kQ },
    { 0, kRunStop },
    { -2, kLastKey }
};

}  // namespace chico
