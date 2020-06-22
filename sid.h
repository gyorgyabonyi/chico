// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_SID_H
#define CHICO_SID_H

#include <cstdint>

namespace chico {

class Sid final {
public:
    void Reset();
    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t data);

private:
    uint8_t registers_[64];
};

}  // namespace chico

#endif  // CHICO_SID_H
