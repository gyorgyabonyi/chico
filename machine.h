// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

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
