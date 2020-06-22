// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_EMULATOR_H
#define CHICO_EMULATOR_H

#include <SDL2/SDL.h>

#include "frame_buffer.h"

namespace chico {

class Config;
class Machine;

class Emulator final {
public:
    Emulator(const Config& config, Machine* machine);
    ~Emulator();

    void PowerUp();
    void Run();

private:
    const Config& config_;
    Machine* machine_;
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    uint32_t start_tick_;
    int ticks_per_frame_;
    FrameBuffer frame_buffer_;

    bool PumpMessages();
    void EmulateFrame();
    void DisplayFrame();
    void Throttle();
};

}  // namespace chico

#endif  // CHICO_EMULATOR_H
