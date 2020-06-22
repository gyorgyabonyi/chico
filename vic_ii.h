// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_VIC_II_H
#define CHICO_VIC_II_H

#include <cstdint>

#include "frame_buffer.h"

namespace chico {

class Config;
class Bus;

class VicII final {
public:
    VicII(const Config& config_, Bus* bus);

    uint8_t Read(uint16_t address) {
        const uint16_t ea = address & 0x3fu;
        const uint8_t value = (this->*kReadTable[ea])(ea);
        return value;
    }
    void Write(uint16_t address, uint8_t data) {
        const uint16_t ea = address & 0x3fu;
        (this->*kWriteTable[ea])(ea, data);
    }

    void Reset();
    void BeginLine(int line, uint8_t* line_buffer);
    int CycleOne(int cycle, uint8_t* line_buffer);

private:
    using ReadFunction = uint8_t (VicII::*)(uint16_t address);
    using WriteFunction = void (VicII::*)(uint16_t address, uint8_t data);

    static const ReadFunction kReadTable[64];
    static const WriteFunction kWriteTable[64];

    const Config &config_;
    Bus* bus_;
    uint8_t registers_[64];
    int raster_irq_;

    int visible_width_;
    int visible_height_;
    int screen_width_;
    int screen_height_;
    uint8_t* pixel_;
    int x_;
    int y_;
    int min_x_;
    int max_x_;
    int min_y_;
    int max_y_;
    uint8_t char_line_[40];
    uint8_t color_line_[40];
    int char_row_;
    uint16_t char_rom_base_;

    uint8_t RenderScreenPixel();

    uint8_t RdReg(uint16_t address);
    uint8_t RdCtrl2(uint16_t address);
    uint8_t RdMp(uint16_t address);
    uint8_t RdIr(uint16_t address);
    uint8_t RdRLo(uint16_t address);
    uint8_t RdMxx(uint16_t address);
    uint8_t RdNil(uint16_t address);
    void WrReg(uint16_t address, uint8_t data);
    void WrCtrl1(uint16_t address, uint8_t data);
    void WrRc(uint16_t address, uint8_t data);
    void WrIr(uint16_t address, uint8_t data);
    void WrIe(uint16_t address, uint8_t data);
    void WrNil(uint16_t address, uint8_t data);
};

}  // namespace chico

#endif  //CHICO_VIC_II_H
