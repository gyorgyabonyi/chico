// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "cia_2.h"

#include "bus.h"

namespace chico {

Cia2::Cia2(Bus *bus)
    :   bus_(bus) {}

void Cia2::UpdateIrqLine(bool state) {
    if (state) {
        bus_->Nmi();
    }
}

uint8_t Cia2::ReadPortA() {
    // TODO(gyorgy): Implement it.
    return 0x00;
}

uint8_t Cia2::ReadPortB() {
    // TODO(gyorgy): Implement it.
    return 0x00;
}

void Cia2::WritePortA(uint8_t data) {
    // TODO(gyorgy): Implement it.
}

void Cia2::WritePortB(uint8_t data) {
    // TODO(gyorgy): Implement it.
}

}  // namespace chico
