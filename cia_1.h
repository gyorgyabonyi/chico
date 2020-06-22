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

#ifndef CHICO_CIA_1_H
#define CHICO_CIA_1_H

#include "cia.h"

namespace chico {

class Bus;
class Keyboard;

class Cia1 final : public Cia {
public:
    Cia1(Bus* bus, Keyboard* keyboard);

protected:
    void UpdateIrqLine(bool state) override;
    uint8_t ReadPortA() override;
    uint8_t ReadPortB() override;
    void WritePortA(uint8_t data) override;
    void WritePortB(uint8_t data) override;

private:
    Bus* bus_;
    Keyboard* keyboard_;
};

}  // namespace chico

#endif  // CHICO_CIA_1_H
