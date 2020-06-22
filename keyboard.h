// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_KEYBOARD_H
#define CHICO_KEYBOARD_H

#include <cstdint>

namespace chico {

class Keyboard final {
public:
    Keyboard();

    void Reset();
    void SetColumns(uint8_t columns);
    uint8_t GetColumns() { return columns_; }
    void SetRows(uint8_t) {}
    uint8_t GetRows();

    void OnKeyDown(int scancode);
    void OnKeyUp(int scancode);

private:
    struct Mapping {
        int host_key_;
        int c64_key_;
    };
    static const Mapping kMappings[];

    uint8_t columns_;
    uint8_t rows_[8];

    int FindMapping(int host_key) const;
};


}  // namespace chico

#endif  // CHICO_KEYBOARD_H
