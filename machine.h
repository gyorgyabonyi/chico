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

#ifndef CHICO_MACHINE_H
#define CHICO_MACHINE_H

#include "bus.h"
#include "cia_1.h"
#include "cia_2.h"
#include "cpu.h"
#include "keyboard.h"
#include "sid.h"
#include "vic_ii.h"

namespace chico {

class Config;

class Machine final {
public:
    Machine(const Config& config);

    constexpr Keyboard* GetKeyboard() { return &keyboard_; }

    void Reset();
    void RunFrame(FrameBuffer* frame_buffer);

private:
    const Config& config_;
    Bus bus_;
    Cia1 cia1_;
    Cia2 cia2_;
    Cpu cpu_;
    Sid sid_;
    VicII vic_;
    Keyboard keyboard_;
    int overflow_cycles_;
};

}  // namespace chico

#endif  // CHICO_MACHINE_H
