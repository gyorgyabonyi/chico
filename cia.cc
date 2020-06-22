// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "cia.h"

#include "logging.h"

namespace chico {

constexpr uint8_t kTA   = (1u << 0u);
constexpr uint8_t kTB   = (1u << 1u);
constexpr uint8_t kALRM = (1u << 2u);
constexpr uint8_t kSP   = (1u << 3u);
constexpr uint8_t kFLG  = (1u << 4u);
constexpr uint8_t kIR   = (1u << 7u);
constexpr uint8_t kSC   = (1u << 7u);

constexpr uint8_t kSTART        = (1u << 0u);
constexpr uint8_t kPB_ON        = (1u << 1u);
constexpr uint8_t kOUTMODE      = (1u << 2u);
constexpr uint8_t kRUNMODE      = (1u << 3u);
constexpr uint8_t kLOAD         = (1u << 4u);
constexpr uint8_t kINMODE       = (1u << 5u);
constexpr uint8_t kINMODE_HI    = (1u << 6u);
constexpr uint8_t kSPMODE       = (1u << 6u);
constexpr uint8_t kTODIN        = (1u << 7u);
constexpr uint8_t kALARM        = (1u << 7u);

void Cia::Reset() {
    irq_state_ = 0;
    irq_mask_ = 0;
    cra_ = 0;
    crb_ = 0;
}

void Cia::UpdateTimers(int elapsed_cycles) {
    if (cra_ & kSTART) {
        UpdateTimerA(elapsed_cycles);
    }
    if ((crb_ & kSTART) && !(crb_ & kINMODE_HI)) {
        UpdateTimerB(elapsed_cycles);
    }
    if (irq_state_ & irq_mask_ & 0x1fu) {
        irq_state_ |= kIR;
        UpdateIrqLine(true);
    } else {
        irq_state_ &= ~kIR;
        UpdateIrqLine(false);
    }
}

void Cia::UpdateClock(int fps) {
    // TODO(gyorgy): Implement it.
}

// CNT input not implemented yet.
// PB lines are not implemented yet.
void Cia::UpdateTimerA(int elapsed_cycles) {
    if (timer_counter_a_ <= elapsed_cycles) {
       irq_state_ |= kTA;
        if (cra_ & kRUNMODE) {
            timer_counter_a_ = 0;
        } else {
            timer_counter_a_ = timer_latch_a_ - (elapsed_cycles - timer_counter_a_);
        }
        if ((crb_ & kSTART) && (crb_ & kINMODE_HI)) {
            UpdateTimerB(1);
        }
    } else {
        timer_counter_a_ -= elapsed_cycles;
    }
}

void Cia::UpdateTimerB(int elapsed_cycles) {
    if (timer_counter_b_ <= elapsed_cycles) {
        irq_state_ |= kTB;
        if (crb_ & kRUNMODE) {
            timer_counter_b_ = 0;
        } else {
            timer_counter_b_ = timer_latch_b_ - (elapsed_cycles - timer_counter_b_);
        }
    } else {
        timer_counter_b_ -= elapsed_cycles;
    }
}

uint8_t Cia::ReadPra() {
    const uint8_t in = ReadPortA() & ~port_a_direction_;
    const uint8_t out = port_a_out_ & port_a_direction_;
    return in | out;
}

uint8_t Cia::ReadPrb() {
    const uint8_t in = ReadPortB() & ~port_b_direction_;
    const uint8_t out = port_b_out_ & port_b_direction_;
    return in | out;
}

uint8_t Cia::ReadDdra() {
    return port_a_direction_;
}

uint8_t Cia::ReadDdrb() {
    return port_b_direction_;
}

uint8_t Cia::ReadTaLo() {
    return timer_counter_a_ & 0xffu;
}

uint8_t Cia::ReadTaHi() {
    return timer_counter_a_ >> 8u;
}

uint8_t Cia::ReadTbLo() {
    return timer_counter_b_ & 0xffu;
}

uint8_t Cia::ReadTbHi() {
    return timer_counter_b_ >> 8u;
}

uint8_t Cia::ReadTod10th() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

uint8_t Cia::ReadTodSec() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

uint8_t Cia::ReadTodMin() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

uint8_t Cia::ReadTodHr() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

uint8_t Cia::ReadSrd() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

uint8_t Cia::ReadIcr() {
    const uint8_t value = irq_state_ & 0x9f;
    irq_state_ = 0;
    UpdateIrqLine(false);
    return value;
}

uint8_t Cia::ReadCra() {
    return cra_ & 0xefu;
}

uint8_t Cia::ReadCrb() {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
    return 0;
}

void Cia::WritePra(uint8_t data) {
    port_a_out_ = data;
    WritePortA(port_a_direction_ & port_a_out_);
}

void Cia::WritePrb(uint8_t data) {
    port_b_out_ = data;
    WritePortB(port_b_direction_ & port_b_out_);
}

void Cia::WriteDdra(uint8_t data) {
    port_a_direction_ = data;
}

void Cia::WriteDdrb(uint8_t data) {
    port_b_direction_ = data;
}

void Cia::WriteTaLo(uint8_t data) {
    timer_latch_a_ = (timer_latch_a_ & 0xff00u) | uint16_t(data);
}

void Cia::WriteTaHi(uint8_t data) {
    timer_latch_a_ = (timer_latch_a_ & 0xffu) | (uint16_t(data) << 8u);
    if (!(cra_ & kSTART)) {
        timer_counter_a_ = timer_latch_a_;
    }
}

void Cia::WriteTbLo(uint8_t data) {
    timer_latch_b_ = (timer_latch_b_ & 0xff00u) | uint16_t(data);
}

void Cia::WriteTbHi(uint8_t data) {
    timer_latch_b_ = (timer_latch_b_ & 0xffu) | (uint16_t(data) << 8u);
    if (!(crb_ & kSTART)) {
        timer_counter_b_ = timer_latch_b_;
    }
}

void Cia::WriteTod10th(uint8_t data) {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
}

void Cia::WriteTodSec(uint8_t data) {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
}

void Cia::WriteTodMin(uint8_t data) {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
}

void Cia::WriteTodHr(uint8_t data) {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
}

void Cia::WriteSrd(uint8_t data) {
    // TODO(gyorgy): Implement it.
    Log(Fatal) << "not implemented";
}

void Cia::WriteIcr(uint8_t data) {
    if (data & kSC) {
        irq_mask_ |= (data & 0x1fu);
    } else {
        irq_mask_ &= (~data & 0x1fu);
    }
}

void Cia::WriteCra(uint8_t data) {
    cra_ = data & ~kLOAD;
    if (data & kLOAD) {
        timer_counter_a_ = timer_latch_a_;
    }
}

void Cia::WriteCrb(uint8_t data) {
    crb_ = data & ~kLOAD;
    if (data & kLOAD) {
        timer_counter_b_ = timer_latch_b_;
    }
}

const Cia::ReadFunction Cia::kReadTable[16] {
    &Cia::ReadPra,     &Cia::ReadPrb,    &Cia::ReadDdra,   &Cia::ReadDdrb,
    &Cia::ReadTaLo,    &Cia::ReadTaHi,   &Cia::ReadTbLo,   &Cia::ReadTbHi,
    &Cia::ReadTod10th, &Cia::ReadTodSec, &Cia::ReadTodMin, &Cia::ReadTodHr,
    &Cia::ReadSrd,     &Cia::ReadIcr,    &Cia::ReadCra,    &Cia::ReadCrb
};

const Cia::WriteFunction Cia::kWriteTable[16] = {
    &Cia::WritePra,     &Cia::WritePrb,    &Cia::WriteDdra,   &Cia::WriteDdrb,
    &Cia::WriteTaLo,    &Cia::WriteTaHi,   &Cia::WriteTbLo,   &Cia::WriteTbHi,
    &Cia::WriteTod10th, &Cia::WriteTodSec, &Cia::WriteTodMin, &Cia::WriteTodHr,
    &Cia::WriteSrd,     &Cia::WriteIcr,    &Cia::WriteCra,    &Cia::WriteCrb
};

}  // namespace chico
