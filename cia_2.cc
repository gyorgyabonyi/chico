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
