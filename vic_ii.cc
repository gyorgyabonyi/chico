// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#include "vic_ii.h"

#include "config.h"
#include "bus.h"

#include "logging.h"

namespace chico {

constexpr uint8_t kIRQ  = 0x80u;
constexpr uint8_t kILP  = 0x08u;
constexpr uint8_t kIMMC = 0x04u;
constexpr uint8_t kIMBC = 0x02u;
constexpr uint8_t kIRST = 0x01u;

constexpr uint8_t kELP  = 0x80u;
constexpr uint8_t kEMMC = 0x04u;
constexpr uint8_t kEMBC = 0x02u;
constexpr uint8_t kERST = 0x01u;

// constexpr int kM0X      = 0x00;
// constexpr int kM0Y      = 0x01;
// constexpr int kM1X      = 0x02;
// constexpr int kM1Y      = 0x03;
// constexpr int kM2X      = 0x04;
// constexpr int kM2Y      = 0x05;
// constexpr int kM3X      = 0x06;
// constexpr int kM3Y      = 0x07;
// constexpr int kM4X      = 0x08;
// constexpr int kM4Y      = 0x09;
// constexpr int kM5X      = 0x0a;
// constexpr int kM5Y      = 0x0b;
// constexpr int kM6X      = 0x0c;
// constexpr int kM6Y      = 0x0d;
// constexpr int kM7X      = 0x0e;
// constexpr int kM7Y      = 0x0f;
// constexpr int kMxX      = 0x10;
constexpr int kCTRL1    = 0x11;
constexpr int kRC       = 0x12;
// constexpr int kLPX      = 0x13;
// constexpr int kLPY      = 0x14;
// constexpr int kMxE      = 0x15;
// constexpr int kCTRL2    = 0x16;
// constexpr int kMxYE     = 0x17;
constexpr int kMP       = 0x18;
constexpr int kIR       = 0x19;
constexpr int kIE       = 0x1a;
// constexpr int kMxDP     = 0x1b;
// constexpr int kMxMC     = 0x1c;
// constexpr int kMxXE     = 0x1d;
// constexpr int kMxM      = 0x1e;
// constexpr int kMxD      = 0x1f;
constexpr int kEC       = 0x20;
constexpr int kB0C      = 0x21;
// constexpr int kB1C      = 0x22;
// constexpr int kB2C      = 0x23;
// constexpr int kB3C      = 0x24;
// constexpr int kMM0      = 0x25;
// constexpr int kMM1      = 0x26;
// constexpr int kM0C      = 0x27;
// constexpr int kM1C      = 0x28;
// constexpr int kM2C      = 0x29;
// constexpr int kM3C      = 0x2a;
// constexpr int kM4C      = 0x2b;
// constexpr int kM5C      = 0x2c;
// constexpr int kM6C      = 0x2d;
// constexpr int kM7C      = 0x2e;

VicII::VicII(const Config& config, Bus* bus)
    :   config_(config),
        bus_(bus),
        raster_irq_(512) {}

void VicII::Reset() {
    raster_irq_ = 512;
    visible_width_ = config_.GetVisiblePixels();
    visible_height_ = config_.GetVisibleLines();
    screen_width_ = 320;
    screen_height_ = 200;
    min_y_ = (visible_height_ - screen_height_) / 2;  // min_y_ = 51;
    max_y_ = min_y_ + screen_height_;
    min_x_ = (visible_width_ - screen_width_) / 2;  // min_x_ = 24;
    max_x_ = min_x_ + screen_width_;
}

void VicII::BeginLine(int line, uint8_t* line_buffer) {
    y_ = line;
    x_ = 0;
    pixel_ = line_buffer;
    registers_[kRC] = line & 0xffu;
    if (line > 0xffu) {
        registers_[kCTRL1] |= 0x80u;
    } else {
        registers_[kCTRL1] &= 0x7fu;
    }
    if (line == raster_irq_) {
        registers_[kIR] |= kIRST;
        if (registers_[kIR] & registers_[kIE] & 0xfu) {
            registers_[kIR] |= kIR;
            bus_->SetIrq(true);
        }
    }

    const int screen_y = (y_ - min_y_);
    char_row_ = screen_y % 8;
    if (char_row_ == 0) {
        // it's a bad line, read line buffers.
        const uint16_t screen_base = (registers_[kMP] & 0xf0u) << 6u;
        const int char_y = screen_y / 8;
        for (int x = 0; x < 40; x++) {
            const uint16_t char_offset = char_y * 40 + x;
            color_line_[x] = bus_->VicReadColor(char_offset);
            char_line_[x] = bus_->VicRead(screen_base + char_offset);
        }
    }
    char_rom_base_ = (registers_[kMP] & 0x0eu) << 10u;
}

int VicII::CycleOne(int cycle, uint8_t* line_buffer) {
    if (y_ >= visible_height_ || x_ >= visible_width_) {
        return 0;
    }
    const uint8_t border_color = registers_[kEC];
    if (y_ < min_y_ || y_ >= max_y_) {
        for (int last_x = std::min(visible_width_, x_ + 8); x_ < last_x; x_++, pixel_++) {
            *pixel_ = border_color;
        }
    } else {
        for (int last_x = std::min(visible_width_, x_ + 8); x_ < last_x; x_++, pixel_++) {
            if (x_ < min_x_ || x_ >= max_x_) {
                *pixel_ = border_color;
            } else {
                *pixel_ = RenderScreenPixel();
            }
        }
    }
    return 0;
}

uint8_t VicII::RenderScreenPixel() {
    const int screen_x = (x_ - min_x_);
    const int char_x = screen_x / 8;
    const uint8_t ch = char_line_[char_x];
    const uint8_t bits = bus_->VicRead(char_rom_base_ + ch * 8 + char_row_);
    const uint8_t mask = 0x80u >> (screen_x & 0x07u);
    if (bits & mask) {
        return color_line_[char_x];
    } else {
        return registers_[kB0C] & 0x0fu;
    }
}

uint8_t VicII::RdReg(uint16_t address) {
    return registers_[address];
}

uint8_t VicII::RdCtrl2(uint16_t address) {
    return registers_[address] | 0xc0u;
}

uint8_t VicII::RdMp(uint16_t address) {
    return registers_[address] | 0x01u;
}

uint8_t VicII::RdIr(uint16_t address) {
    return registers_[address] | 0x70u;
}

uint8_t VicII::RdRLo(uint16_t address) {
    return registers_[address] | 0xf0u;
}

uint8_t VicII::RdMxx(uint16_t address) {
    const uint8_t result = registers_[address];
    registers_[address] = 0;
    return result;
}

uint8_t VicII::RdNil(uint16_t) {
    return 0xffu;
}

void VicII::WrReg(uint16_t address, uint8_t data) {
    registers_[address] = data;
}

void VicII::WrCtrl1(uint16_t address, uint8_t data) {
    if (data & 0x80u) {
        raster_irq_ |= 0x100u;
    } else {
        raster_irq_ &= 0xffu;
    }
    registers_[address] = data;
}

void VicII::WrRc(uint16_t, uint8_t data) {
    raster_irq_ = (raster_irq_ & 0x100u) | data;
}

void VicII::WrIr(uint16_t address, uint8_t data) {
    uint8_t result = registers_[kIR] & ~data & 0x0fu;
    if (!(result & 0x0fu)) {
        result &= 0x0fu;
        bus_->SetIrq(false);
    }
    registers_[address] = result;
}

void VicII::WrNil(uint16_t, uint8_t) {}

const VicII::ReadFunction VicII::kReadTable[64] = {
    &VicII::RdReg,  // 0x00 M0X
    &VicII::RdReg,  // 0x01 M0Y
    &VicII::RdReg,  // 0x02 M1X
    &VicII::RdReg,  // 0x03 M1Y
    &VicII::RdReg,  // 0x04 M2X
    &VicII::RdReg,  // 0x05 M2Y
    &VicII::RdReg,  // 0x06 M3X
    &VicII::RdReg,  // 0x07 M3Y
    &VicII::RdReg,  // 0x08 M4X
    &VicII::RdReg,  // 0x09 M4Y
    &VicII::RdReg,  // 0x0a M5X
    &VicII::RdReg,  // 0x0b M5Y
    &VicII::RdReg,  // 0x0c M6X
    &VicII::RdReg,  // 0x0d M6Y
    &VicII::RdReg,  // 0x0e M7X
    &VicII::RdReg,  // 0x0f M7Y
    &VicII::RdReg,  // 0x10 MxX
    &VicII::RdReg,  // 0x11 CTRL1
    &VicII::RdReg,  // 0x12 RC
    &VicII::RdReg,  // 0x13 LPX
    &VicII::RdReg,  // 0x14 LPY
    &VicII::RdReg,  // 0x15 MxE
    &VicII::RdCtrl2,  // 0x16 CTRL2
    &VicII::RdReg,  // 0x17 MxYE
    &VicII::RdMp,  // 0x18 MP
    &VicII::RdIr,  // 0x19 IR
    &VicII::RdRLo,  // 0x1a IE
    &VicII::RdReg,  // 0x1b MxDP
    &VicII::RdReg,  // 0x1c MxMC
    &VicII::RdReg,  // 0x1d MxXE
    &VicII::RdMxx,  // 0x1e MxM
    &VicII::RdMxx,  // 0x1f MxD
    &VicII::RdRLo,  // 0x20 EC
    &VicII::RdRLo,  // 0x21 B0C
    &VicII::RdRLo,  // 0x22 B1C
    &VicII::RdRLo,  // 0x23 B2C
    &VicII::RdRLo,  // 0x24 B3C
    &VicII::RdRLo,  // 0x25 MM0
    &VicII::RdRLo,  // 0x26 MM1
    &VicII::RdRLo,  // 0x27 M0C
    &VicII::RdRLo,  // 0x28 M1C
    &VicII::RdRLo,  // 0x29 M2C
    &VicII::RdRLo,  // 0x2a M3C
    &VicII::RdRLo,  // 0x2b M4C
    &VicII::RdRLo,  // 0x2c M5C
    &VicII::RdRLo,  // 0x2d M6C
    &VicII::RdRLo,  // 0x2e M7C
    &VicII::RdNil,  // 0x2f
    &VicII::RdNil,  // 0x30
    &VicII::RdNil,  // 0x31
    &VicII::RdNil,  // 0x32
    &VicII::RdNil,  // 0x33
    &VicII::RdNil,  // 0x34
    &VicII::RdNil,  // 0x35
    &VicII::RdNil,  // 0x36
    &VicII::RdNil,  // 0x37
    &VicII::RdNil,  // 0x38
    &VicII::RdNil,  // 0x39
    &VicII::RdNil,  // 0x3a
    &VicII::RdNil,  // 0x3b
    &VicII::RdNil,  // 0x3c
    &VicII::RdNil,  // 0x3d
    &VicII::RdNil,  // 0x3e
    &VicII::RdNil,  // 0x3f
};

const VicII::WriteFunction VicII::kWriteTable[64] = {
    &VicII::WrReg,  // 0x00 M0X
    &VicII::WrReg,  // 0x01 M0Y
    &VicII::WrReg,  // 0x02 M1X
    &VicII::WrReg,  // 0x03 M1Y
    &VicII::WrReg,  // 0x04 M2X
    &VicII::WrReg,  // 0x05 M2Y
    &VicII::WrReg,  // 0x06 M3X
    &VicII::WrReg,  // 0x07 M3Y
    &VicII::WrReg,  // 0x08 M4X
    &VicII::WrReg,  // 0x09 M4Y
    &VicII::WrReg,  // 0x0a M5X
    &VicII::WrReg,  // 0x0b M5Y
    &VicII::WrReg,  // 0x0c M6X
    &VicII::WrReg,  // 0x0d M6Y
    &VicII::WrReg,  // 0x0e M7X
    &VicII::WrReg,  // 0x0f M7Y
    &VicII::WrReg,  // 0x10 MxX
    &VicII::WrCtrl1,  // 0x11 CTRL1
    &VicII::WrRc,   // 0x12 RC
    &VicII::WrReg,  // 0x13 LPX
    &VicII::WrReg,  // 0x14 LPY
    &VicII::WrReg,  // 0x15 MxE
    &VicII::WrReg,  // 0x16 CTRL2
    &VicII::WrReg,  // 0x17 MxYE
    &VicII::WrReg,  // 0x18 MP
    &VicII::WrIr,   // 0x19 IR
    &VicII::WrReg,  // 0x1a IE
    &VicII::WrReg,  // 0x1b MxDP
    &VicII::WrReg,  // 0x1c MxMC
    &VicII::WrReg,  // 0x1d MxXE
    &VicII::WrNil,  // 0x1e MxM
    &VicII::WrNil,  // 0x1f MxD
    &VicII::WrReg,  // 0x20 EC
    &VicII::WrReg,  // 0x21 B0C
    &VicII::WrReg,  // 0x22 B1C
    &VicII::WrReg,  // 0x23 B2C
    &VicII::WrReg,  // 0x24 B3C
    &VicII::WrReg,  // 0x25 MM0
    &VicII::WrReg,  // 0x26 MM1
    &VicII::WrReg,  // 0x27 M0C
    &VicII::WrReg,  // 0x28 M1C
    &VicII::WrReg,  // 0x29 M2C
    &VicII::WrReg,  // 0x2a M3C
    &VicII::WrReg,  // 0x2b M4C
    &VicII::WrReg,  // 0x2c M5C
    &VicII::WrReg,  // 0x2d M6C
    &VicII::WrReg,  // 0x2e M7C
    &VicII::WrNil,  // 0x2f
    &VicII::WrNil,  // 0x30
    &VicII::WrNil,  // 0x31
    &VicII::WrNil,  // 0x32
    &VicII::WrNil,  // 0x33
    &VicII::WrNil,  // 0x34
    &VicII::WrNil,  // 0x35
    &VicII::WrNil,  // 0x36
    &VicII::WrNil,  // 0x37
    &VicII::WrNil,  // 0x38
    &VicII::WrNil,  // 0x39
    &VicII::WrNil,  // 0x3a
    &VicII::WrNil,  // 0x3b
    &VicII::WrNil,  // 0x3c
    &VicII::WrNil,  // 0x3d
    &VicII::WrNil,  // 0x3e
    &VicII::WrNil,  // 0x3f
};

}  // namespace chico
