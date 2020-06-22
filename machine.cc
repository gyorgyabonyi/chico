// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

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
