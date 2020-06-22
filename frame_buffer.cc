// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include <assert.h>
#include "frame_buffer.h"

namespace chico {

FrameBuffer::FrameBuffer()
    :   width_(0),
        height_(0),
        pixels_(nullptr) {}

FrameBuffer::~FrameBuffer() {
    delete [] pixels_;
}

void FrameBuffer::Reset(int width, int height) {
    assert(width > 0);
    assert(width <= kPitch);
    assert(height > 0);

    delete [] pixels_;
    width_ = width;
    height_ = height;
    pixels_ = new uint8_t[kPitch * height];
}

}  // namespace chico