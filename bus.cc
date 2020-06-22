// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "bus.h"

#include "cia_1.h"
#include "cia_2.h"
#include "cpu.h"
#include "sid.h"
#include "vic_ii.h"

namespace chico {

Bus::Bus(Cia1* cia1, Cia2* cia2, Cpu* cpu, Sid* sid, VicII* vic,
         const uint8_t* basic_rom, const uint8_t* kernal_rom, const uint8_t* char_rom)
    :   cia1_(cia1),
        cia2_(cia2),
        cpu_(cpu),
        sid_(sid),
        vic_(vic),
        basic_rom_(basic_rom),
        kernal_rom_(kernal_rom),
        char_rom_(char_rom) {}

void Bus::Nmi() {
    cpu_->Nmi();
}

void Bus::SetIrq(bool value) {
    cpu_->SetIrqSignal(value);
}

uint8_t Bus::ReadRam(uint16_t address) {
    return ram_[address];
}

uint8_t Bus::ReadBasicRom(uint16_t address) {
    return basic_rom_[address & 0x1fffu];
}

uint8_t Bus::ReadKernalRom(uint16_t address) {
    return kernal_rom_[address & 0x1fffu];
}

uint8_t Bus::ReadCharRom(uint16_t address) {
    return char_rom_[address & 0x0fffu];
}

uint8_t Bus::ReadIo(uint16_t address) {
    return (this->*kIoReadTable[(address >> 8u) & 0xfu])(address);
}

uint8_t Bus::ReadVic(uint16_t address) {
    return vic_->Read(address);
}

uint8_t Bus::ReadSid(uint16_t address) {
    return sid_->Read(address);
}

uint8_t Bus::ReadColorRam(uint16_t address) {
    return color_ram_[address & 0x3ffu] & 0x0f;
}

uint8_t Bus::ReadCia1(uint16_t address) {
    return cia1_->Read(address);
}

uint8_t Bus::ReadCia2(uint16_t address) {
    return cia2_->Read(address);
}

uint8_t Bus::ReadIo1(uint16_t) {
    return 0;
}

uint8_t Bus::ReadIo2(uint16_t) {
    return 0;
}

void Bus::WriteRam(uint16_t address, uint8_t data) {
    ram_[address] = data;
}

void Bus::WriteIo(uint16_t address, uint8_t data) {
    (this->*kIoWriteTable[(address >> 8u) & 0xfu])(address, data);
}

void Bus::WriteVic(uint16_t address, uint8_t data) {
    vic_->Write(address, data);
}

void Bus::WriteSid(uint16_t address, uint8_t data) {
    sid_->Write(address, data);
}

void Bus::WriteColorRam(uint16_t address, uint8_t data) {
    color_ram_[address & 0x3ffu] = data & 0x0fu;
}

void Bus::WriteCia1(uint16_t address, uint8_t data) {
    cia1_->Write(address, data);
}

void Bus::WriteCia2(uint16_t address, uint8_t data) {
    cia2_->Write(address, data);
}

void Bus::WriteIo1(uint16_t, uint8_t) {}

void Bus::WriteIo2(uint16_t, uint8_t) {}

const Bus::ReadFunction Bus::kCpuReadTable[16][8] = {
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadBasicRom,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadBasicRom},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadBasicRom,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadBasicRom},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam,       &Bus::ReadRam},
    {&Bus::ReadRam, &Bus::ReadCharRom, &Bus::ReadCharRom,   &Bus::ReadCharRom,
     &Bus::ReadRam, &Bus::ReadIo,      &Bus::ReadIo,        &Bus::ReadIo},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadKernalRom, &Bus::ReadKernalRom,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadKernalRom, &Bus::ReadKernalRom},
    {&Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadKernalRom, &Bus::ReadKernalRom,
     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadKernalRom, &Bus::ReadKernalRom}
};

const Bus::WriteFunction Bus::kCpuWriteTable[16][8] = {
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteIo,  &Bus::WriteIo,  &Bus::WriteIo},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam},
    {&Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam,
     &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam, &Bus::WriteRam}
};

const Bus::ReadFunction Bus::kIoReadTable[16] = {
    &Bus::ReadVic,      &Bus::ReadVic,      &Bus::ReadVic,      &Bus::ReadVic,
    &Bus::ReadSid,      &Bus::ReadSid,      &Bus::ReadSid,      &Bus::ReadSid,
    &Bus::ReadColorRam, &Bus::ReadColorRam, &Bus::ReadColorRam, &Bus::ReadColorRam,
    &Bus::ReadCia1,     &Bus::ReadCia2,     &Bus::ReadIo1,      &Bus::ReadIo2
};

const Bus::WriteFunction Bus::kIoWriteTable[16] = {
    &Bus::WriteVic,      &Bus::WriteVic,      &Bus::WriteVic,      &Bus::WriteVic,
    &Bus::WriteSid,      &Bus::WriteSid,      &Bus::WriteSid,      &Bus::WriteSid,
    &Bus::WriteColorRam, &Bus::WriteColorRam, &Bus::WriteColorRam, &Bus::WriteColorRam,
    &Bus::WriteCia1,     &Bus::WriteCia2,     &Bus::WriteIo1,      &Bus::WriteIo2
};

const Bus::ReadFunction  Bus::kVicReadTable[16][4] = {
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadCharRom, &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadCharRom, &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam },
    {&Bus::ReadRam,     &Bus::ReadRam, &Bus::ReadRam,     &Bus::ReadRam }
};

}  // namespace chico
