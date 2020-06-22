// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "cpu.h"

#include "bus.h"
#include "logging.h"

namespace chico {

constexpr uint16_t kNmiVector   = 0xfffau;
constexpr uint16_t kResetVector = 0xfffcu;
constexpr uint16_t kIrqVector   = 0xfffeu;
constexpr uint16_t kStackBase = 0x0100u;

constexpr uint8_t kFlagC = 0x01u;
constexpr uint8_t kFlagZ = 0x02u;
constexpr uint8_t kFlagI = 0x04u;
constexpr uint8_t kFlagD = 0x08u;
constexpr uint8_t kFlagB = 0x10u;
constexpr uint8_t kFlagU = 0x20u;
constexpr uint8_t kFlagV = 0x40u;
constexpr uint8_t kFlagN = 0x80u;

constexpr uint8_t kIrqSignal = 0x01u;
constexpr uint8_t kNmiSignal = 0x02u;

Cpu::Cpu(Bus* bus)
    :   bus_(bus) {}

void Cpu::UpdateFlagC(uint8_t value) {
    p_ = (p_ & ~kFlagC) | (value & kFlagC);
}

void Cpu::UpdateFlagZ(uint8_t value) {
    if (value) {
        p_ &= ~kFlagZ;
    } else {
        p_ |= kFlagZ;
    }
}

void Cpu::UpdateFlagV(uint8_t r, uint8_t v_1, uint8_t v_2) {
    if ((r ^ v_1) & (r ^ v_2) & 0x80u) {
        p_ |= kFlagV;
    } else {
        p_ &= ~kFlagV;
    }
}

void Cpu::UpdateFlagN(uint8_t value) {
    p_ = (p_ & ~kFlagN) | (value & kFlagN);
}

void Cpu::Branch(uint16_t address) {
    penalty_cycles_ = ((pc_ >> 8u) == (address >> 8u)) ? 2 : 1;
    pc_ = address;
}

void Cpu::Write8(uint16_t address, uint8_t data) {
    if (address < 2u) {
        port_[address] = data;
        bus_->SetCpuBank((~port_[0] | (port_[0] & port_[1])) & 7u);
    }
    bus_->CpuWrite(address, data);
}

uint8_t Cpu::Read8(uint16_t address) {
    return bus_->CpuRead(address);
}

uint16_t Cpu::Read16(uint16_t address) {
    return uint16_t(Read8(address) | (uint16_t(Read8(address + 1u)) << 8u));
}

void Cpu::Push8(uint8_t data) {
    Write8(kStackBase + uint16_t(s_), data);
    s_ -= 1u;
}

void Cpu::Push16(uint16_t data) {
    Push8(data >> 8u);
    Push8(data & 0xffu);
}

uint8_t Cpu::Pop8() {
    s_ += 1u;
    return Read8(kStackBase + uint16_t(s_));
}

uint16_t Cpu::Pop16() {
    return uint16_t(Pop8()) | (uint16_t(Pop8()) << 8u);
}

void Cpu::Reset() {
    Write8(0, 0);
    pc_ = Read16(kResetVector);
    s_ = 0xfdu;
    p_ |= (kFlagU | kFlagI);
    irq_signals_ = 0;
}


int Cpu::CycleOne() {
    int done_cycles;
    if (irq_signals_) {
        if (irq_signals_ & kNmiSignal) {
            irq_signals_ &= ~kNmiSignal;
            Push16(pc_);
            Push8(p_);
            p_ |= kFlagI;
            pc_ = Read16(kNmiVector);
            return 7;
        } else if ((irq_signals_ & kIrqSignal) && !(p_ & kFlagI)) {
            Push16(pc_);
            Push8(p_);
            p_ |= kFlagI;
            pc_ = Read16(kIrqVector);
            return 7;
        }
    }
    const uint8_t opcode = Read8(pc_);
    pc_ += 1u;
    penalty_cycles_ = 0;
    return (this->*kOpcodeTable[opcode])() + penalty_cycles_;
}

void Cpu::Nmi() {
    irq_signals_ |= kNmiSignal;
}

void Cpu::SetIrqSignal(bool value) {
    if (value) {
        irq_signals_ |= kIrqSignal;
    } else {
        irq_signals_ &= ~kIrqSignal;
    }
}


uint16_t Cpu::AddrAbs() {
    const uint16_t ea = Read16(pc_);
    pc_ += 2u;
    return ea;
}

uint16_t Cpu::AddrAbx() {
    const uint16_t ba = Read16(pc_);
    const uint16_t ea = ba + uint16_t(x_);
    pc_ += 2u;
    if (ba >> 8u != ea >> 8u) {
        penalty_cycles_ = 1;
    }
    return ea;
}

uint16_t Cpu::AddrAby() {
    const uint16_t ba = Read16(pc_);
    const uint16_t ea = ba + uint16_t(y_);
    pc_ += 2u;
    if (ba >> 8u != ea >> 8u) {
        penalty_cycles_ = 1;
    }
    return ea;
}

uint16_t Cpu::AddrImm() {
    const uint16_t ea = pc_;
    pc_ += 1u;
    return ea;
}

uint16_t Cpu::AddrInd() {
    const uint16_t ba = Read16(pc_);
    pc_ += 2u;
    const uint16_t ea = Read16(ba);
    return ea;
}

uint16_t Cpu::AddrInx() {
    const uint16_t ba = uint16_t(Read8(pc_) + x_) & 0x00ffu;
    pc_ += 1u;
    const uint16_t lo = Read8(ba);
    const uint16_t hi = Read8((ba + 1u) & 0xffu) << 8u;
    const uint16_t ea = lo | hi;
    return ea;
}

uint16_t Cpu::AddrIny() {
    const uint16_t ba = uint16_t(Read8(pc_));
    pc_ += 1u;
    const uint16_t lo = Read8(ba);
    const uint16_t hi = Read8((ba + 1u) & 0xffu) << 8u;
    const uint16_t ea = lo + hi + (uint16_t)y_;
    if (ea >> 8u != hi) {
        penalty_cycles_ = 1;
    }
    return ea;
}

uint16_t Cpu::AddrRel() {
    const uint16_t r = Read8(pc_);
    pc_ += 1u;
    const uint16_t ea = pc_ + (r & 0x80u ? r | 0xff00u : r);
    return ea;
}

uint16_t Cpu::AddrZpg() {
    const uint16_t ea = Read8(pc_);
    pc_ += 1u;
    return ea;
}

uint16_t Cpu::AddrZpx() {
    const uint16_t ea = uint16_t(Read8(pc_) + uint16_t(x_)) & 0x00ffu;
    pc_ += 1u;
    return ea;
}

uint16_t Cpu::AddrZpy() {
    const uint16_t ea = uint16_t(Read8(pc_) + uint16_t(y_)) & 0x00ffu;
    pc_ += 1u;
    return ea;
}

void Cpu::InstKil() {
    Log(Fatal) << "CPU stalled at " << std::hex << pc_;
}

void Cpu::InstAdc(uint16_t address) {
    const uint8_t value = Read8(address);
    if (p_ & kFlagD) {
        Log(Fatal) << "Not implemented yet";
    } else {
        const uint16_t result = a_ + value + (p_ & kFlagC);
        UpdateFlagC(result >> 8u);
        UpdateFlagV(uint8_t(result & 0xffu), a_, value);
        a_ = result & 0xffu;
        UpdateFlagN(a_);
        UpdateFlagZ(a_);
    }
}

void Cpu::InstAnd(uint16_t address) {
    a_ = a_ & Read8(address);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstAsl(uint16_t address) {
    const uint8_t value = Read8(address);
    UpdateFlagC(value >> 7u);
    const uint8_t result = value << 1u;
    UpdateFlagZ(result);
    UpdateFlagN(result);
    Write8(address, result);
}

void Cpu::InstAslAcc() {
    UpdateFlagC(a_ >> 7u);
    a_ <<= 1u;
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstBcc(uint16_t address) {
    if (!(p_ & kFlagC)) {
        Branch(address);
    }
}

void Cpu::InstBcs(uint16_t address) {
    if (p_ & kFlagC) {
        Branch(address);
    }
}

void Cpu::InstBeq(uint16_t address) {
    if (p_ & kFlagZ) {
        Branch(address);
    }
}

void Cpu::InstBit(uint16_t address) {
    const uint8_t value = Read8(address);
    p_ = (p_ & 0x3fu) | (value & 0xc0u);
    UpdateFlagZ(a_ & value);
}

void Cpu::InstBmi(uint16_t address) {
    if (p_ & kFlagN) {
        Branch(address);
    }
}

void Cpu::InstBne(uint16_t address) {
    if (!(p_ & kFlagZ)) {
        Branch(address);
    }
}

void Cpu::InstBpl(uint16_t address) {
    if (!(p_ & kFlagN)) {
        Branch(address);
    }
}

void Cpu::InstBrk() {
    Log(Fatal) << "brk: " << std::hex << pc_;
    Push16(pc_ + 1u);
    Push8(p_  | kFlagB);
    p_ |= kFlagI;
    pc_ = Read16(kIrqVector);
}

void Cpu::InstBvc(uint16_t address) {
    if (!(p_ & kFlagV)) {
        Branch(address);
    }
}

void Cpu::InstBvs(uint16_t address) {
    if (p_ & kFlagV) {
        Branch(address);
    }
}

void Cpu::InstClc() {
    p_ &= ~kFlagC;
}

void Cpu::InstCld() {
    p_ &= ~kFlagD;
}

void Cpu::InstCli() {
    p_ &= ~kFlagI;
}

void Cpu::InstClv() {
    p_ &= ~kFlagV;
}

void Cpu::InstCmp(uint16_t address) {
    const uint8_t value = Read8(address);
    if (a_ >= value) {
        p_ |= kFlagC;
    } else {
        p_ &= ~kFlagC;
    }
    const uint8_t result = a_ - value;
    UpdateFlagZ(result);
    UpdateFlagN(result);
}

void Cpu::InstCpx(uint16_t address) {
    const uint8_t value = Read8(address);
    if (x_ >= value) {
        p_ |= kFlagC;
    } else {
        p_ &= ~kFlagC;
    }
    const uint8_t result = x_ - value;
    UpdateFlagZ(result);
    UpdateFlagN(result);
}

void Cpu::InstCpy(uint16_t address) {
    const uint8_t value = Read8(address);
    if (y_ >= value) {
        p_ |= kFlagC;
    } else {
        p_ &= ~kFlagC;
    }
    const uint8_t result = y_ - value;
    UpdateFlagZ(result);
    UpdateFlagN(result);
}

void Cpu::InstDec(uint16_t address) {
    const uint8_t result = Read8(address) - 1u;
    UpdateFlagZ(result);
    UpdateFlagN(result);
    Write8(address, result);
}

void Cpu::InstDex() {
    x_ -= 1u;
    UpdateFlagZ(x_);
    UpdateFlagN(x_);
}

void Cpu::InstDey() {
    y_ -= 1u;
    UpdateFlagZ(y_);
    UpdateFlagN(y_);
}

void Cpu::InstEor(uint16_t address) {
    a_ ^= Read8(address);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstInc(uint16_t address) {
    const uint8_t result = Read8(address) + 1u;
    UpdateFlagZ(result);
    UpdateFlagN(result);
    Write8(address, result);
}

void Cpu::InstInx() {
    x_ += 1u;
    UpdateFlagZ(x_);
    UpdateFlagN(x_);
}

void Cpu::InstIny() {
    y_ += 1u;
    UpdateFlagZ(y_);
    UpdateFlagN(y_);
}

void Cpu::InstJmp(uint16_t address) {
    pc_ = address;
}

void Cpu::InstJsr(uint16_t address) {
    Push16(pc_ - 1u);
    pc_ = address;
}

void Cpu::InstLda(uint16_t address) {
    a_ = Read8(address);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstLdx(uint16_t address) {
    x_ = Read8(address);
    UpdateFlagZ(x_);
    UpdateFlagN(x_);
}

void Cpu::InstLdy(uint16_t address) {
    y_ = Read8(address);
    UpdateFlagZ(y_);
    UpdateFlagN(y_);
}

void Cpu::InstLsr(uint16_t address) {
    const uint8_t value = Read8(address);
    UpdateFlagC(value);
    const uint8_t result = value >> 1u;
    UpdateFlagZ(result);
    p_ &= ~kFlagN;
    Write8(address, result);
}

void Cpu::InstLsrAcc() {
    UpdateFlagC(a_);
    a_ >>= 1u;
    UpdateFlagZ(a_);
    p_ &= ~kFlagN;
}

void Cpu::InstNop() {}

void Cpu::InstOra(uint16_t address) {
    a_ |= Read8(address);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstPha() {
    Push8(a_);
}

void Cpu::InstPhp() {
    Push8(p_);
}

void Cpu::InstPla() {
    a_ = Pop8();
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstPlp() {
    p_ = Pop8() | kFlagU;
}

void Cpu::InstRol(uint16_t address) {
    const uint8_t value = Read8(address);
    const uint8_t result = (value << 1u) | (p_ & kFlagC);
    UpdateFlagC(value >> 7u);
    UpdateFlagZ(result);
    UpdateFlagN(result);
    Write8(address, result);
}

void Cpu::InstRolAcc() {
    const uint8_t value = a_;
    a_ = (value << 1u) | (p_ & kFlagC);
    UpdateFlagC(value >> 7u);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstRor(uint16_t address) {
    const uint8_t value = Read8(address);
    const uint8_t result = (value >> 1u) | ((p_ & kFlagC) << 7u);
    UpdateFlagC(value);
    UpdateFlagZ(result);
    UpdateFlagN(result);
    Write8(address, result);
}

void Cpu::InstRorAcc() {
    const uint8_t value = a_;
    a_ = (value >> 1u) | ((p_ & kFlagC) << 7u);
    UpdateFlagC(value);
    UpdateFlagZ(a_);
    UpdateFlagN(a_);
}

void Cpu::InstRti() {
    p_ = Pop8() | kFlagU;
    pc_ = Pop16();
}

void Cpu::InstRts() {
    pc_ = Pop16() + 1u;
}

void Cpu::InstSbc(uint16_t address) {
    if (p_ & kFlagD) {
        Log(Fatal) << "BCD mode not implemented yet";
    } else {
        const uint8_t value = Read8(address);
        const uint16_t result = a_ - value - (~p_ & kFlagC);
        UpdateFlagC(~result >> 15u);
        UpdateFlagV(result, a_, value);
        a_ = result & 0xffu;
        UpdateFlagN(a_);
        UpdateFlagZ(a_);
    }
}

void Cpu::InstSec() {
    p_ |= kFlagC;
}

void Cpu::InstSed() {
    p_ |= kFlagD;
}

void Cpu::InstSei() {
    p_ |= kFlagI;
}

void Cpu::InstSta(uint16_t address) {
    Write8(address, a_);
}

void Cpu::InstStx(uint16_t address) {
    Write8(address, x_);
}

void Cpu::InstSty(uint16_t address) {
    Write8(address, y_);
}

void Cpu::InstTax() {
    x_ = a_;
    UpdateFlagN(x_);
    UpdateFlagZ(x_);
}

void Cpu::InstTay() {
    y_ = a_;
    UpdateFlagN(y_);
    UpdateFlagZ(y_);
}

void Cpu::InstTsx() {
    x_ = s_;
    UpdateFlagN(x_);
    UpdateFlagZ(x_);
}

void Cpu::InstTxa() {
    a_ = x_;
    UpdateFlagN(a_);
    UpdateFlagZ(a_);
}

void Cpu::InstTxs() {
    s_ = x_;
}

void Cpu::InstTya() {
    a_ = y_;
    UpdateFlagN(a_);
    UpdateFlagZ(a_);
}

int Cpu::Op00() { InstBrk(); return 7; }
int Cpu::Op01() { InstOra(AddrZpx()); return 6; }
int Cpu::Op02() { InstKil(); return 1; }
int Cpu::Op03() { InstKil(); return 8; }
int Cpu::Op04() { InstKil(); return 3; }
int Cpu::Op05() { InstOra(AddrZpg()); return 3; }
int Cpu::Op06() { InstAsl(AddrZpg()); return 5; }
int Cpu::Op07() { InstKil(); return 5; }
int Cpu::Op08() { InstPhp(); return 3; }
int Cpu::Op09() { InstOra(AddrImm()); return 2; }
int Cpu::Op0a() { InstAslAcc(); return 2; }
int Cpu::Op0b() { InstKil(); return 2; }
int Cpu::Op0c() { InstKil(); return 4; }
int Cpu::Op0d() { InstOra(AddrAbs()); return 4; }
int Cpu::Op0e() { InstAsl(AddrAbs()); return 6; }
int Cpu::Op0f() { InstKil(); return 6; }
int Cpu::Op10() { InstBpl(AddrRel()); return 2; }
int Cpu::Op11() { InstOra(AddrIny()); return 5; }
int Cpu::Op12() { InstKil(); return 1; }
int Cpu::Op13() { InstKil(); return 8; }
int Cpu::Op14() { InstKil(); return 4; }
int Cpu::Op15() { InstOra(AddrZpx()); return 4; }
int Cpu::Op16() { InstAsl(AddrZpx()); return 6; }
int Cpu::Op17() { InstKil(); return 6; }
int Cpu::Op18() { InstClc(); return 2; }
int Cpu::Op19() { InstOra(AddrAby()); return 4; }
int Cpu::Op1a() { InstKil(); return 2; }
int Cpu::Op1b() { InstKil(); return 7; }
int Cpu::Op1c() { InstKil(); return 4; }
int Cpu::Op1d() { InstOra(AddrAbx()); return 4; }
int Cpu::Op1e() { InstAsl(AddrAbx()); return 7; }
int Cpu::Op1f() { InstKil(); return 7; }
int Cpu::Op20() { InstJsr(AddrAbs()); return 6; }
int Cpu::Op21() { InstAnd(AddrInx()); return 6; }
int Cpu::Op22() { InstKil(); return 1; }
int Cpu::Op23() { InstKil(); return 8; }
int Cpu::Op24() { InstBit(AddrZpg()); return 4; }
int Cpu::Op25() { InstAnd(AddrZpg()); return 4; }
int Cpu::Op26() { InstRol(AddrZpg()); return 6; }
int Cpu::Op27() { InstKil(); return 6; }
int Cpu::Op28() { InstPlp(); return 2; }
int Cpu::Op29() { InstAnd(AddrImm()); return 4; }
int Cpu::Op2a() { InstRolAcc(); return 2; }
int Cpu::Op2b() { InstKil(); return 7; }
int Cpu::Op2c() { InstBit(AddrAbs()); return 4; }
int Cpu::Op2d() { InstAnd(AddrAbs()); return 4; }
int Cpu::Op2e() { InstRol(AddrAbs()); return 7; }
int Cpu::Op2f() { InstKil(); return 7; }
int Cpu::Op30() { InstBmi(AddrRel()); return 2; }
int Cpu::Op31() { InstAnd(AddrIny()); return 5; }
int Cpu::Op32() { InstKil(); return 1; }
int Cpu::Op33() { InstKil(); return 8; }
int Cpu::Op34() { InstKil(); return 4; }
int Cpu::Op35() { InstAnd(AddrZpx()); return 4; }
int Cpu::Op36() { InstRol(AddrZpx()); return 6; }
int Cpu::Op37() { InstKil(); return 6; }
int Cpu::Op38() { InstSec(); return 2; }
int Cpu::Op39() { InstAnd(AddrAby()); return 4; }
int Cpu::Op3a() { InstKil(); return 2; }
int Cpu::Op3b() { InstKil(); return 7; }
int Cpu::Op3c() { InstKil(); return 4; }
int Cpu::Op3d() { InstAnd(AddrAbx()); return 4; }
int Cpu::Op3e() { InstRol(AddrAbx()); return 7; }
int Cpu::Op3f() { InstKil(); return 7; }
int Cpu::Op40() { InstRti(); return 6; }
int Cpu::Op41() { InstEor(AddrInx()); return 6; }
int Cpu::Op42() { InstKil(); return 1; }
int Cpu::Op43() { InstKil(); return 8; }
int Cpu::Op44() { InstKil(); return 3; }
int Cpu::Op45() { InstEor(AddrZpg()); return 3; }
int Cpu::Op46() { InstLsr(AddrZpg()); return 5; }
int Cpu::Op47() { InstKil(); return 5; }
int Cpu::Op48() { InstPha(); return 3; }
int Cpu::Op49() { InstEor(AddrImm()); return 2; }
int Cpu::Op4a() { InstLsrAcc(); return 2; }
int Cpu::Op4b() { InstKil(); return 2; }
int Cpu::Op4c() { InstJmp(AddrAbs()); return 3; }
int Cpu::Op4d() { InstEor(AddrAbs()); return 4; }
int Cpu::Op4e() { InstLsr(AddrAbs()); return 6; }
int Cpu::Op4f() { InstKil(); return 7; }
int Cpu::Op50() { InstBvc(AddrRel()); return 2; }
int Cpu::Op51() { InstEor(AddrIny()); return 5; }
int Cpu::Op52() { InstKil(); return 1; }
int Cpu::Op53() { InstKil(); return 8; }
int Cpu::Op54() { InstKil(); return 4; }
int Cpu::Op55() { InstEor(AddrZpx()); return 4; }
int Cpu::Op56() { InstLsr(AddrZpx()); return 6; }
int Cpu::Op57() { InstKil(); return 6; }
int Cpu::Op58() { InstCli(); return 2; }
int Cpu::Op59() { InstEor(AddrAby()); return 4; }
int Cpu::Op5a() { InstKil(); return 2; }
int Cpu::Op5b() { InstKil(); return 7; }
int Cpu::Op5c() { InstKil(); return 4; }
int Cpu::Op5d() { InstEor(AddrAbx()); return 4; }
int Cpu::Op5e() { InstLsr(AddrAbx()); return 7; }
int Cpu::Op5f() { InstKil(); return 7; }
int Cpu::Op60() { InstRts(); return 6; }
int Cpu::Op61() { InstAdc(AddrInx()); return 6; }
int Cpu::Op62() { InstKil(); return 1; }
int Cpu::Op63() { InstKil(); return 8; }
int Cpu::Op64() { InstKil(); return 3; }
int Cpu::Op65() { InstAdc(AddrZpg()); return 3; }
int Cpu::Op66() { InstRor(AddrZpg()); return 5; }
int Cpu::Op67() { InstKil(); return 5; }
int Cpu::Op68() { InstPla(); return 4; }
int Cpu::Op69() { InstAdc(AddrImm()); return 2; }
int Cpu::Op6a() { InstRorAcc(); return 2; }
int Cpu::Op6b() { InstKil(); return 2; }
int Cpu::Op6c() { InstJmp(AddrInd()); return 5; }
int Cpu::Op6d() { InstAdc(AddrAbs()); return 4; }
int Cpu::Op6e() { InstRor(AddrAbs()); return 6; }
int Cpu::Op6f() { InstKil(); return 6; }
int Cpu::Op70() { InstBvs(AddrRel()); return 2; }
int Cpu::Op71() { InstAdc(AddrIny()); return 5; }
int Cpu::Op72() { InstKil(); return 1; }
int Cpu::Op73() { InstKil(); return 8; }
int Cpu::Op74() { InstKil(); return 4; }
int Cpu::Op75() { InstAdc(AddrZpx()); return 4; }
int Cpu::Op76() { InstRor(AddrZpx()); return 6; }
int Cpu::Op77() { InstKil(); return 6; }
int Cpu::Op78() { InstSei(); return 2; }
int Cpu::Op79() { InstAdc(AddrAby()); return 4; }
int Cpu::Op7a() { InstKil(); return 2; }
int Cpu::Op7b() { InstKil(); return 7; }
int Cpu::Op7c() { InstKil(); return 4; }
int Cpu::Op7d() { InstAdc(AddrAbx()); return 4; }
int Cpu::Op7e() { InstRor(AddrAbx()); return 7; }
int Cpu::Op7f() { InstKil(); return 7; }
int Cpu::Op80() { InstKil(); return 2; }
int Cpu::Op81() { InstSta(AddrInx()); return 6; }
int Cpu::Op82() { InstKil(); return 2; }
int Cpu::Op83() { InstKil(); return 6; }
int Cpu::Op84() { InstSty(AddrZpg()); return 3; }
int Cpu::Op85() { InstSta(AddrZpg()); return 3; }
int Cpu::Op86() { InstStx(AddrZpg()); return 3; }
int Cpu::Op87() { InstKil(); return 3; }
int Cpu::Op88() { InstDey(); return 2; }
int Cpu::Op89() { InstKil(); return 2; }
int Cpu::Op8a() { InstTxa(); return 2; }
int Cpu::Op8b() { InstKil(); return 2; }
int Cpu::Op8c() { InstSty(AddrAbs()); return 4; }
int Cpu::Op8d() { InstSta(AddrAbs()); return 4; }
int Cpu::Op8e() { InstStx(AddrAbs()); return 4; }
int Cpu::Op8f() { InstKil(); return 4; }
int Cpu::Op90() { InstBcc(AddrRel()); return 2; }
int Cpu::Op91() { InstSta(AddrIny()); return 6; }
int Cpu::Op92() { InstKil(); return 1; }
int Cpu::Op93() { InstKil(); return 6; }
int Cpu::Op94() { InstSty(AddrZpx()); return 4; }
int Cpu::Op95() { InstSta(AddrZpx()); return 4; }
int Cpu::Op96() { InstStx(AddrZpy()); return 4; }
int Cpu::Op97() { InstKil(); return 4; }
int Cpu::Op98() { InstTya(); return 2; }
int Cpu::Op99() { InstSta(AddrAby()); return 5; }
int Cpu::Op9a() { InstTxs(); return 2; }
int Cpu::Op9b() { InstKil(); return 5; }
int Cpu::Op9c() { InstKil(); return 5; }
int Cpu::Op9d() { InstSta(AddrAbx()); return 5; }
int Cpu::Op9e() { InstKil(); return 5; }
int Cpu::Op9f() { InstKil(); return 5; }
int Cpu::Opa0() { InstLdy(AddrImm()); return 2; }
int Cpu::Opa1() { InstLda(AddrInx()); return 6; }
int Cpu::Opa2() { InstLdx(AddrImm()); return 2; }
int Cpu::Opa3() { InstKil(); return 6; }
int Cpu::Opa4() { InstLdy(AddrZpg()); return 3; }
int Cpu::Opa5() { InstLda(AddrZpg()); return 3; }
int Cpu::Opa6() { InstLdx(AddrZpg()); return 3; }
int Cpu::Opa7() { InstKil(); return 3; }
int Cpu::Opa8() { InstTay(); return 2; }
int Cpu::Opa9() { InstLda(AddrImm()); return 2; }
int Cpu::Opaa() { InstTax(); return 2; }
int Cpu::Opab() { InstKil(); return 2; }
int Cpu::Opac() { InstLdy(AddrAbs()); return 4; }
int Cpu::Opad() { InstLda(AddrAbs()); return 4; }
int Cpu::Opae() { InstLdx(AddrAbs()); return 4; }
int Cpu::Opaf() { InstKil(); return 4; }
int Cpu::Opb0() { InstBcs(AddrRel()); return 2; }
int Cpu::Opb1() { InstLda(AddrIny()); return 5; }
int Cpu::Opb2() { InstKil(); return 1; }
int Cpu::Opb3() { InstKil(); return 5; }
int Cpu::Opb4() { InstLdy(AddrZpx()); return 4; }
int Cpu::Opb5() { InstLda(AddrZpx()); return 4; }
int Cpu::Opb6() { InstLdx(AddrZpy()); return 4; }
int Cpu::Opb7() { InstKil(); return 4; }
int Cpu::Opb8() { InstClv(); return 2; }
int Cpu::Opb9() { InstLda(AddrAby()); return 4; }
int Cpu::Opba() { InstTsx(); return 2; }
int Cpu::Opbb() { InstKil(); return 4; }
int Cpu::Opbc() { InstLdy(AddrAbx()); return 4; }
int Cpu::Opbd() { InstLda(AddrAbx()); return 4; }
int Cpu::Opbe() { InstLdx(AddrAby()); return 4; }
int Cpu::Opbf() { InstKil(); return 4; }
int Cpu::Opc0() { InstCpy(AddrImm()); return 2; }
int Cpu::Opc1() { InstCmp(AddrInx()); return 6; }
int Cpu::Opc2() { InstKil(); return 2; }
int Cpu::Opc3() { InstKil(); return 8; }
int Cpu::Opc4() { InstCpy(AddrZpg()); return 3; }
int Cpu::Opc5() { InstCmp(AddrZpg()); return 3; }
int Cpu::Opc6() { InstDec(AddrZpg()); return 5; }
int Cpu::Opc7() { InstKil(); return 5; }
int Cpu::Opc8() { InstIny(); return 2; }
int Cpu::Opc9() { InstCmp(AddrImm()); return 2; }
int Cpu::Opca() { InstDex(); return 2; }
int Cpu::Opcb() { InstKil(); return 2; }
int Cpu::Opcc() { InstCpy(AddrAbs()); return 4; }
int Cpu::Opcd() { InstCmp(AddrAbs()); return 4; }
int Cpu::Opce() { InstDec(AddrAbs()); return 6; }
int Cpu::Opcf() { InstKil(); return 6; }
int Cpu::Opd0() { InstBne(AddrRel()); return 2; }
int Cpu::Opd1() { InstCmp(AddrIny()); return 5; }
int Cpu::Opd2() { InstKil(); return 1; }
int Cpu::Opd3() { InstKil(); return 8; }
int Cpu::Opd4() { InstKil(); return 4; }
int Cpu::Opd5() { InstCmp(AddrZpx()); return 4; }
int Cpu::Opd6() { InstDec(AddrZpx()); return 6; }
int Cpu::Opd7() { InstKil(); return 6; }
int Cpu::Opd8() { InstCld(); return 2; }
int Cpu::Opd9() { InstCmp(AddrAby()); return 4; }
int Cpu::Opda() { InstKil(); return 2; }
int Cpu::Opdb() { InstKil(); return 7; }
int Cpu::Opdc() { InstKil(); return 4; }
int Cpu::Opdd() { InstCmp(AddrAbx()); return 4; }
int Cpu::Opde() { InstDec(AddrAbx()); return 7; }
int Cpu::Opdf() { InstKil(); return 7; }
int Cpu::Ope0() { InstCpx(AddrImm()); return 2; }
int Cpu::Ope1() { InstSbc(AddrInx()); return 6; }
int Cpu::Ope2() { InstKil(); return 2; }
int Cpu::Ope3() { InstKil(); return 8; }
int Cpu::Ope4() { InstCpx(AddrZpg()); return 3; }
int Cpu::Ope5() { InstSbc(AddrZpg()); return 3; }
int Cpu::Ope6() { InstInc(AddrZpg()); return 5; }
int Cpu::Ope7() { InstKil(); return 5; }
int Cpu::Ope8() { InstInx(); return 2; }
int Cpu::Ope9() { InstSbc(AddrImm()); return 2; }
int Cpu::Opea() { InstNop(); return 2; }
int Cpu::Opeb() { InstKil(); return 2; }
int Cpu::Opec() { InstCpx(AddrAbs()); return 4; }
int Cpu::Oped() { InstSbc(AddrAbs()); return 4; }
int Cpu::Opee() { InstInc(AddrAbs()); return 6; }
int Cpu::Opef() { InstKil(); return 6; }
int Cpu::Opf0() { InstBeq(AddrRel()); return 2; }
int Cpu::Opf1() { InstSbc(AddrIny()); return 5; }
int Cpu::Opf2() { InstKil(); return 1; }
int Cpu::Opf3() { InstKil(); return 8; }
int Cpu::Opf4() { InstKil(); return 4; }
int Cpu::Opf5() { InstSbc(AddrZpx()); return 4; }
int Cpu::Opf6() { InstInc(AddrZpx()); return 6; }
int Cpu::Opf7() { InstKil(); return 6; }
int Cpu::Opf8() { InstSed(); return 2; }
int Cpu::Opf9() { InstSbc(AddrAby()); return 4; }
int Cpu::Opfa() { InstKil(); return 2; }
int Cpu::Opfb() { InstKil(); return 7; }
int Cpu::Opfc() { InstKil(); return 4; }
int Cpu::Opfd() { InstSbc(AddrAby()); return 4; }
int Cpu::Opfe() { InstInc(AddrAbx()); return 7; }
int Cpu::Opff() { InstKil(); return 7; }

const Cpu::Opcode Cpu::kOpcodeTable[256] = {
  &Cpu::Op00, &Cpu::Op01, &Cpu::Op02, &Cpu::Op03, &Cpu::Op04, &Cpu::Op05, &Cpu::Op06, &Cpu::Op07,
  &Cpu::Op08, &Cpu::Op09, &Cpu::Op0a, &Cpu::Op0b, &Cpu::Op0c, &Cpu::Op0d, &Cpu::Op0e, &Cpu::Op0f,
  &Cpu::Op10, &Cpu::Op11, &Cpu::Op12, &Cpu::Op13, &Cpu::Op14, &Cpu::Op15, &Cpu::Op16, &Cpu::Op17,
  &Cpu::Op18, &Cpu::Op19, &Cpu::Op1a, &Cpu::Op1b, &Cpu::Op1c, &Cpu::Op1d, &Cpu::Op1e, &Cpu::Op1f,
  &Cpu::Op20, &Cpu::Op21, &Cpu::Op22, &Cpu::Op23, &Cpu::Op24, &Cpu::Op25, &Cpu::Op26, &Cpu::Op27,
  &Cpu::Op28, &Cpu::Op29, &Cpu::Op2a, &Cpu::Op2b, &Cpu::Op2c, &Cpu::Op2d, &Cpu::Op2e, &Cpu::Op2f,
  &Cpu::Op30, &Cpu::Op31, &Cpu::Op32, &Cpu::Op33, &Cpu::Op34, &Cpu::Op35, &Cpu::Op36, &Cpu::Op37,
  &Cpu::Op38, &Cpu::Op39, &Cpu::Op3a, &Cpu::Op3b, &Cpu::Op3c, &Cpu::Op3d, &Cpu::Op3e, &Cpu::Op3f,
  &Cpu::Op40, &Cpu::Op41, &Cpu::Op42, &Cpu::Op43, &Cpu::Op44, &Cpu::Op45, &Cpu::Op46, &Cpu::Op47,
  &Cpu::Op48, &Cpu::Op49, &Cpu::Op4a, &Cpu::Op4b, &Cpu::Op4c, &Cpu::Op4d, &Cpu::Op4e, &Cpu::Op4f,
  &Cpu::Op50, &Cpu::Op51, &Cpu::Op52, &Cpu::Op53, &Cpu::Op54, &Cpu::Op55, &Cpu::Op56, &Cpu::Op57,
  &Cpu::Op58, &Cpu::Op59, &Cpu::Op5a, &Cpu::Op5b, &Cpu::Op5c, &Cpu::Op5d, &Cpu::Op5e, &Cpu::Op5f,
  &Cpu::Op60, &Cpu::Op61, &Cpu::Op62, &Cpu::Op63, &Cpu::Op64, &Cpu::Op65, &Cpu::Op66, &Cpu::Op67,
  &Cpu::Op68, &Cpu::Op69, &Cpu::Op6a, &Cpu::Op6b, &Cpu::Op6c, &Cpu::Op6d, &Cpu::Op6e, &Cpu::Op6f,
  &Cpu::Op70, &Cpu::Op71, &Cpu::Op72, &Cpu::Op73, &Cpu::Op74, &Cpu::Op75, &Cpu::Op76, &Cpu::Op77,
  &Cpu::Op78, &Cpu::Op79, &Cpu::Op7a, &Cpu::Op7b, &Cpu::Op7c, &Cpu::Op7d, &Cpu::Op7e, &Cpu::Op7f,
  &Cpu::Op80, &Cpu::Op81, &Cpu::Op82, &Cpu::Op83, &Cpu::Op84, &Cpu::Op85, &Cpu::Op86, &Cpu::Op87,
  &Cpu::Op88, &Cpu::Op89, &Cpu::Op8a, &Cpu::Op8b, &Cpu::Op8c, &Cpu::Op8d, &Cpu::Op8e, &Cpu::Op8f,
  &Cpu::Op90, &Cpu::Op91, &Cpu::Op92, &Cpu::Op93, &Cpu::Op94, &Cpu::Op95, &Cpu::Op96, &Cpu::Op97,
  &Cpu::Op98, &Cpu::Op99, &Cpu::Op9a, &Cpu::Op9b, &Cpu::Op9c, &Cpu::Op9d, &Cpu::Op9e, &Cpu::Op9f,
  &Cpu::Opa0, &Cpu::Opa1, &Cpu::Opa2, &Cpu::Opa3, &Cpu::Opa4, &Cpu::Opa5, &Cpu::Opa6, &Cpu::Opa7,
  &Cpu::Opa8, &Cpu::Opa9, &Cpu::Opaa, &Cpu::Opab, &Cpu::Opac, &Cpu::Opad, &Cpu::Opae, &Cpu::Opaf,
  &Cpu::Opb0, &Cpu::Opb1, &Cpu::Opb2, &Cpu::Opb3, &Cpu::Opb4, &Cpu::Opb5, &Cpu::Opb6, &Cpu::Opb7,
  &Cpu::Opb8, &Cpu::Opb9, &Cpu::Opba, &Cpu::Opbb, &Cpu::Opbc, &Cpu::Opbd, &Cpu::Opbe, &Cpu::Opbf,
  &Cpu::Opc0, &Cpu::Opc1, &Cpu::Opc2, &Cpu::Opc3, &Cpu::Opc4, &Cpu::Opc5, &Cpu::Opc6, &Cpu::Opc7,
  &Cpu::Opc8, &Cpu::Opc9, &Cpu::Opca, &Cpu::Opcb, &Cpu::Opcc, &Cpu::Opcd, &Cpu::Opce, &Cpu::Opcf,
  &Cpu::Opd0, &Cpu::Opd1, &Cpu::Opd2, &Cpu::Opd3, &Cpu::Opd4, &Cpu::Opd5, &Cpu::Opd6, &Cpu::Opd7,
  &Cpu::Opd8, &Cpu::Opd9, &Cpu::Opda, &Cpu::Opdb, &Cpu::Opdc, &Cpu::Opdd, &Cpu::Opde, &Cpu::Opdf,
  &Cpu::Ope0, &Cpu::Ope1, &Cpu::Ope2, &Cpu::Ope3, &Cpu::Ope4, &Cpu::Ope5, &Cpu::Ope6, &Cpu::Ope7,
  &Cpu::Ope8, &Cpu::Ope9, &Cpu::Opea, &Cpu::Opeb, &Cpu::Opec, &Cpu::Oped, &Cpu::Opee, &Cpu::Opef,
  &Cpu::Opf0, &Cpu::Opf1, &Cpu::Opf2, &Cpu::Opf3, &Cpu::Opf4, &Cpu::Opf5, &Cpu::Opf6, &Cpu::Opf7,
  &Cpu::Opf8, &Cpu::Opf9, &Cpu::Opfa, &Cpu::Opfb, &Cpu::Opfc, &Cpu::Opfd, &Cpu::Opfe, &Cpu::Opff,
};

}  // namespace chico
