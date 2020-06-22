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
