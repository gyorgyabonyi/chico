// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

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
