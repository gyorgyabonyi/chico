// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "cia_1.h"

#include "bus.h"
#include "keyboard.h"

namespace chico {

Cia1::Cia1(Bus *bus, Keyboard* keyboard)
    :   bus_(bus),
        keyboard_(keyboard) {}

void Cia1::UpdateIrqLine(bool state) {
    bus_->SetIrq(state);
}

uint8_t Cia1::ReadPortA() {
    return keyboard_->GetColumns();
}

uint8_t Cia1::ReadPortB() {
    return keyboard_->GetRows();
}

void Cia1::WritePortA(uint8_t data) {
    keyboard_->SetColumns(data);
}

void Cia1::WritePortB(uint8_t data) {
    keyboard_->SetRows(data);
}

}  // namespace chico
