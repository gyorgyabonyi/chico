// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_BUS_H
#define CHICO_BUS_H

#include <cstdint>

namespace chico {

class Cia1;
class Cia2;
class Cpu;
class Sid;
class VicII;

class Bus final {
public:
    Bus(Cia1* cia1, Cia2* cia2, Cpu* cpu, Sid* sid, VicII* vic,
        const uint8_t* basic_rom, const uint8_t* kernal_rom, const uint8_t* char_rom);

    void CpuWrite(uint16_t address, uint8_t data) {
        (this->*kCpuWriteTable[address >> 12u][cpu_bank_])(address, data);
    }

    uint8_t CpuRead(uint16_t address) {
        return (this->*kCpuReadTable[address >> 12u][cpu_bank_])(address);
    }

    uint8_t VicRead(uint16_t address) {
        const uint16_t ea = (vic_bank_ << 14u) | (address & 0x3fffu);
        return (this->*kVicReadTable[ea >> 12][vic_bank_])(address);
    }

    uint8_t VicReadColor(uint16_t address) {
        return color_ram_[address & 0x03ffu] & 0x0fu;
    }

    void Nmi();
    void SetIrq(bool value);

    constexpr void SetCpuBank(uint8_t cpu_bank) { cpu_bank_ = cpu_bank; }

private:
    using ReadFunction = uint8_t (Bus::*)(uint16_t address);
    using WriteFunction = void (Bus::*)(uint16_t address, uint8_t data);

    static const ReadFunction kCpuReadTable[16][8];
    static const WriteFunction kCpuWriteTable[16][8];
    static const ReadFunction kIoReadTable[16];
    static const WriteFunction kIoWriteTable[16];
    static const ReadFunction kVicReadTable[16][4];

    Cia1* cia1_;
    Cia2* cia2_;
    Cpu* cpu_;
    Sid* sid_;
    VicII* vic_;
    const uint8_t* basic_rom_;
    const uint8_t* kernal_rom_;
    const uint8_t* char_rom_;
    int cpu_bank_;
    int vic_bank_;
    uint8_t ram_[65536];
    uint8_t color_ram_[1024];

    uint8_t ReadRam(uint16_t address);
    uint8_t ReadBasicRom(uint16_t address);
    uint8_t ReadKernalRom(uint16_t address);
    uint8_t ReadCharRom(uint16_t address);
    uint8_t ReadIo(uint16_t address);
    uint8_t ReadVic(uint16_t address);
    uint8_t ReadSid(uint16_t address);
    uint8_t ReadColorRam(uint16_t address);
    uint8_t ReadCia1(uint16_t address);
    uint8_t ReadCia2(uint16_t address);
    uint8_t ReadIo1(uint16_t address);
    uint8_t ReadIo2(uint16_t address);
    void WriteRam(uint16_t address, uint8_t data);
    void WriteIo(uint16_t address, uint8_t data);
    void WriteVic(uint16_t address, uint8_t data);
    void WriteSid(uint16_t address, uint8_t data);
    void WriteColorRam(uint16_t address, uint8_t data);
    void WriteCia1(uint16_t address, uint8_t data);
    void WriteCia2(uint16_t address, uint8_t data);
    void WriteIo1(uint16_t address, uint8_t data);
    void WriteIo2(uint16_t address, uint8_t data);
};

}  // namespace chico

#endif  // CHICO_BUS_H
