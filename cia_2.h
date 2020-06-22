// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO__CIA_2_H
#define CHICO__CIA_2_H

#include "cia.h"

namespace chico {

class Bus;

class Cia2 final : public Cia {
public:
    Cia2(Bus* bus);

protected:
    void UpdateIrqLine(bool state) override;
    uint8_t ReadPortA() override;
    uint8_t ReadPortB() override;
    void WritePortA(uint8_t data) override;
    void WritePortB(uint8_t data) override;

private:
    Bus* bus_;
};

}

#endif  // CHICO__CIA_2_H
