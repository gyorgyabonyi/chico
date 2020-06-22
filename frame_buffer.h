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

#ifndef CHICO__FRAME_BUFFER_H
#define CHICO__FRAME_BUFFER_H

#include <cstdint>

namespace chico {

class FrameBuffer final {
public:
    FrameBuffer();
    ~FrameBuffer();

    constexpr int width() const { return width_; }
    constexpr int height() const { return height_; }
    constexpr uint8_t* line(int line) { return pixels_ + line * kPitch; }
    constexpr const uint8_t* line(int line) const { return pixels_ + line * kPitch; }
    constexpr uint8_t* pixel(int x, int y) { return pixels_ + y * kPitch + x; }

    void Reset(int width, int height);

private:
    static constexpr int kPitch = 512;

    int width_;
    int height_;
    uint8_t* pixels_;
};

}  // namespace chico

#endif  // CHICO__FRAME_BUFFER_H
