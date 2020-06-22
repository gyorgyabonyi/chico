// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "sid.h"

#include "logging.h"

namespace chico {

void Sid::Reset() {
    // TODO(gyorgy): Implement it.
}

uint8_t Sid::Read(uint16_t address) const {
    // TODO(gyorgy): Implemented it.
    return registers_[address & 0x1fu];
}

void Sid::Write(uint16_t address, uint8_t data) {
    // TODO(gyorgy): Implemented it.
    registers_[address & 0x1fu] = data;
}


}  // namespace chico
