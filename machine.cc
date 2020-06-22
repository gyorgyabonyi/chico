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

#include "machine.h"

#include "config.h"

namespace chico {

Machine::Machine(const chico::Config &config)
    :   config_(config),
        bus_(&cia1_,
             &cia2_,
             &cpu_,
             &sid_,
             &vic_,
             config_.GetBasicRom(),
             config_.GetKernalRom(),
             config.GetCharRom()),
        cia1_(&bus_, &keyboard_),
        cia2_(&bus_),
        cpu_(&bus_),
        vic_(config, &bus_) {}

void Machine::Reset() {
    overflow_cycles_ = 0;
    cia1_.Reset();
    cia2_.Reset();
    cpu_.Reset();
    sid_.Reset();
    vic_.Reset();
    keyboard_.Reset();
}

void Machine::RunFrame(FrameBuffer* frame_buffer) {
    const int cpu_cycles_per_line = config_.GetCyclesPerLine();
    const int total_lines = config_.GetTotalLines();
    for (int line = 0; line < total_lines; line++) {
        uint8_t* line_buffer = frame_buffer->line(line);
        vic_.BeginLine(line, line_buffer);
        int cpu_cycle = overflow_cycles_;
        int vic_cycle = 0;
        while (cpu_cycle < cpu_cycles_per_line) {
            int start_cycles = cpu_cycle;
            while (vic_cycle <= cpu_cycle) {
                cpu_cycle += vic_.CycleOne(vic_cycle, line_buffer);
                vic_cycle += 1;
            }
            cpu_cycle += cpu_.CycleOne();
            const int elapsed_cycles = cpu_cycle - start_cycles;
            cia1_.UpdateTimers(elapsed_cycles);
            cia2_.UpdateTimers(elapsed_cycles);
        }
        overflow_cycles_ = cpu_cycle - cpu_cycles_per_line;
    }
    // TODO(gyorgy): Update CIA real time clocks.
}

}  // namespace chico
