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

#include "emulator.h"

#include "config.h"
#include "machine.h"

namespace chico {

static const uint32_t kPalette[16] = {
    0x00000000u, 0xFFFFFF00u, 0x68372B00u, 0x70A4B200u,
    0x6F3D8600u, 0x588D4300u, 0x35287900u, 0xB8C76F00u,
    0x6F4F2500u, 0x43390000u, 0x9A675900u, 0x44444400u,
    0x6C6C6C00u, 0x9AD28400u, 0x6C5EB500u, 0x95959500u,
};

Emulator::Emulator(const Config& config, Machine* machine)
    :   config_(config),
        machine_(machine),
        window_(nullptr),
        renderer_(nullptr),
        texture_(nullptr),
        start_tick_(0),
        ticks_per_frame_(0) {}

Emulator::~Emulator() {
    SDL_DestroyTexture(texture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Emulator::PowerUp() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    const int width = config_.GetVisiblePixels();
    const int height = config_.GetVisibleLines();
    const int magnification = config_.GetScreenMagnification();
    window_ = SDL_CreateWindow("Chico",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               width * magnification,
                               height * magnification,
                               0);
    renderer_ = SDL_CreateRenderer(window_, -1, 0);
    texture_ = SDL_CreateTexture(renderer_,
                                 SDL_PIXELFORMAT_RGBX8888,
                                 // SDL_PIXELFORMAT_INDEX8,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 width,
                                 height);
    const int cycles_per_frame = config_.GetTotalLines() * config_.GetCyclesPerLine();
    const double frames_per_second = double(config_.GetCpuClock()) / double(cycles_per_frame);
    ticks_per_frame_ = int(1000.0 / frames_per_second);
    frame_buffer_.Reset(width, height);
    machine_->Reset();
}

void Emulator::Run() {
    while(PumpMessages()) {
        EmulateFrame();
        DisplayFrame();
        Throttle();
    }
}

bool Emulator::PumpMessages() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
                machine_->GetKeyboard()->OnKeyDown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                machine_->GetKeyboard()->OnKeyUp(event.key.keysym.scancode);
                break;
        }
    }
    return true;
}

void Emulator::EmulateFrame() {
    machine_->RunFrame(&frame_buffer_);
}

void Emulator::DisplayFrame() {
    void* pixels;
    int pitch;
    SDL_LockTexture(texture_, NULL, &pixels, &pitch);
    const int width = config_.GetVisiblePixels();
    const int height = config_.GetVisibleLines();
    for (int line = 0; line < height; line++) {
        uint32_t* dest = (uint32_t*)((char*)pixels + line * pitch);
        const uint8_t* src = frame_buffer_.line(line);
        for (int x = 0; x < width; x++) {
            *dest++ = kPalette[*src++ & 0x0fu];
        }
    }
    SDL_UnlockTexture(texture_);
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
    SDL_RenderPresent(renderer_);
}

void Emulator::Throttle() {
    const int elapsed_ticks = int(SDL_GetTicks() - start_tick_);
    const int delay_ticks = ticks_per_frame_ - elapsed_ticks;
    if (delay_ticks > 0) {
        SDL_Delay(delay_ticks);
    }
    start_tick_ = SDL_GetTicks();
}

}  // namespace chico
