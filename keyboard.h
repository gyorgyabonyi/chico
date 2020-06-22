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
