// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "config.h"
#include "emulator.h"
#include "machine.h"

int main(int argc, char** argv) {
    chico::Config config;
    config.Load();
    chico::Machine machine(config);
    chico::Emulator emulator(config, &machine);
    emulator.PowerUp();
    emulator.Run();
    return 0;
}

