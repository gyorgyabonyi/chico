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

#ifndef CHICO_CIA_H
#define CHICO_CIA_H

#include <cstdint>

namespace chico {

class Cia {
public:
    virtual ~Cia() = default;

    uint8_t Read(uint16_t address) {
        return (this->*kReadTable[address & 0xfu])();
    }
    void Write(uint16_t address, uint8_t data) {
        (this->*kWriteTable[address & 0xfu])(data);
    }

    void Reset();
    void UpdateTimers(int elapsed_cycles);
    void UpdateClock(int fps);

protected:
    uint8_t port_a_out_;
    uint8_t port_a_direction_;
    uint8_t port_b_out_;
    uint8_t port_b_direction_;
    uint16_t timer_counter_a_;
    uint16_t timer_latch_a_;
    uint16_t timer_counter_b_;
    uint16_t timer_latch_b_;
    uint8_t irq_state_;
    uint8_t irq_mask_;
    uint8_t cra_;
    uint8_t crb_;

    virtual void UpdateIrqLine(bool state) = 0;
    virtual uint8_t ReadPortA() = 0;
    virtual uint8_t ReadPortB() = 0;
    virtual void WritePortA(uint8_t data) = 0;
    virtual void WritePortB(uint8_t data) = 0;

private:
    using ReadFunction = uint8_t (Cia::*)();
    using WriteFunction = void (Cia::*)(uint8_t data);

    static const ReadFunction kReadTable[16];
    static const WriteFunction kWriteTable[16];

    void UpdateTimerA(int elapsed_cycles);
    void UpdateTimerB(int elapsed_cycles);

    uint8_t ReadPra();
    uint8_t ReadPrb();
    uint8_t ReadDdra();
    uint8_t ReadDdrb();
    uint8_t ReadTaLo();
    uint8_t ReadTaHi();
    uint8_t ReadTbLo();
    uint8_t ReadTbHi();
    uint8_t ReadTod10th();
    uint8_t ReadTodSec();
    uint8_t ReadTodMin();
    uint8_t ReadTodHr();
    uint8_t ReadSrd();
    uint8_t ReadIcr();
    uint8_t ReadCra();
    uint8_t ReadCrb();
    void WritePra(uint8_t data);
    void WritePrb(uint8_t data);
    void WriteDdra(uint8_t data);
    void WriteDdrb(uint8_t data);
    void WriteTaLo(uint8_t data);
    void WriteTaHi(uint8_t data);
    void WriteTbLo(uint8_t data);
    void WriteTbHi(uint8_t data);
    void WriteTod10th(uint8_t data);
    void WriteTodSec(uint8_t data);
    void WriteTodMin(uint8_t data);
    void WriteTodHr(uint8_t data);
    void WriteSrd(uint8_t data);
    void WriteIcr(uint8_t data);
    void WriteCra(uint8_t data);
    void WriteCrb(uint8_t data);
};

}  // namespace chico

#endif  // CHICO_CIA_H
