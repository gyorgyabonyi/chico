// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

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
