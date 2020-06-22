// Copyright (c) 2020 Gyorgy Abonyi. All Rights Reserved.

#ifndef CHICO_CPU_H
#define CHICO_CPU_H

#include <cstdint>

namespace chico {

class Bus;

class Cpu final {
public:
    Cpu(Bus* bus);

    void Reset();
    int CycleOne();
    void Nmi();
    void SetIrqSignal(bool value);

private:
    using Opcode = int (Cpu::*)();

    static const Opcode kOpcodeTable[256];

    Bus* bus_;
    uint8_t a_;
    uint8_t x_;
    uint8_t y_;
    uint8_t s_;
    uint8_t p_;
    uint16_t pc_;
    uint8_t port_[2];
    uint8_t irq_signals_;
    int penalty_cycles_;

    void UpdateFlagC(uint8_t value);
    void UpdateFlagZ(uint8_t value);
    void UpdateFlagV(uint8_t r, uint8_t v_1, uint8_t v_2);
    void UpdateFlagN(uint8_t value);
    void Branch(uint16_t address);
    void Write8(uint16_t address, uint8_t data);
    uint8_t Read8(uint16_t address);
    uint16_t Read16(uint16_t address);
    void Push8(uint8_t data);
    void Push16(uint16_t data);
    uint8_t Pop8();
    uint16_t Pop16();

    uint16_t AddrAbs();
    uint16_t AddrAbx();
    uint16_t AddrAby();
    uint16_t AddrImm();
    uint16_t AddrInd();
    uint16_t AddrInx();
    uint16_t AddrIny();
    uint16_t AddrRel();
    uint16_t AddrZpg();
    uint16_t AddrZpx();
    uint16_t AddrZpy();

    void InstKil();
    void InstAdc(uint16_t address);
    void InstAnd(uint16_t address);
    void InstAsl(uint16_t address);
    void InstAslAcc();
    void InstBcc(uint16_t address);
    void InstBcs(uint16_t address);
    void InstBeq(uint16_t address);
    void InstBit(uint16_t address);
    void InstBmi(uint16_t address);
    void InstBne(uint16_t address);
    void InstBpl(uint16_t address);
    void InstBrk();
    void InstBvc(uint16_t address);
    void InstBvs(uint16_t address);
    void InstClc();
    void InstCld();
    void InstCli();
    void InstClv();
    void InstCmp(uint16_t address);
    void InstCpx(uint16_t address);
    void InstCpy(uint16_t address);
    void InstDec(uint16_t address);
    void InstDex();
    void InstDey();
    void InstEor(uint16_t address);
    void InstInc(uint16_t address);
    void InstInx();
    void InstIny();
    void InstJmp(uint16_t address);
    void InstJsr(uint16_t address);
    void InstLda(uint16_t address);
    void InstLdx(uint16_t address);
    void InstLdy(uint16_t address);
    void InstLsr(uint16_t address);
    void InstLsrAcc();
    void InstNop();
    void InstOra(uint16_t address);
    void InstPha();
    void InstPhp();
    void InstPla();
    void InstPlp();
    void InstRol(uint16_t address);
    void InstRolAcc();
    void InstRor(uint16_t address);
    void InstRorAcc();
    void InstRti();
    void InstRts();
    void InstSbc(uint16_t address);
    void InstSec();
    void InstSed();
    void InstSei();
    void InstSta(uint16_t address);
    void InstStx(uint16_t address);
    void InstSty(uint16_t address);
    void InstTax();
    void InstTay();
    void InstTsx();
    void InstTxa();
    void InstTxs();
    void InstTya();

    int Op00(); int Op01(); int Op02(); int Op03(); int Op04(); int Op05(); int Op06(); int Op07();
    int Op08(); int Op09(); int Op0a(); int Op0b(); int Op0c(); int Op0d(); int Op0e(); int Op0f();
    int Op10(); int Op11(); int Op12(); int Op13(); int Op14(); int Op15(); int Op16(); int Op17();
    int Op18(); int Op19(); int Op1a(); int Op1b(); int Op1c(); int Op1d(); int Op1e(); int Op1f();
    int Op20(); int Op21(); int Op22(); int Op23(); int Op24(); int Op25(); int Op26(); int Op27();
    int Op28(); int Op29(); int Op2a(); int Op2b(); int Op2c(); int Op2d(); int Op2e(); int Op2f();
    int Op30(); int Op31(); int Op32(); int Op33(); int Op34(); int Op35(); int Op36(); int Op37();
    int Op38(); int Op39(); int Op3a(); int Op3b(); int Op3c(); int Op3d(); int Op3e(); int Op3f();
    int Op40(); int Op41(); int Op42(); int Op43(); int Op44(); int Op45(); int Op46(); int Op47();
    int Op48(); int Op49(); int Op4a(); int Op4b(); int Op4c(); int Op4d(); int Op4e(); int Op4f();
    int Op50(); int Op51(); int Op52(); int Op53(); int Op54(); int Op55(); int Op56(); int Op57();
    int Op58(); int Op59(); int Op5a(); int Op5b(); int Op5c(); int Op5d(); int Op5e(); int Op5f();
    int Op60(); int Op61(); int Op62(); int Op63(); int Op64(); int Op65(); int Op66(); int Op67();
    int Op68(); int Op69(); int Op6a(); int Op6b(); int Op6c(); int Op6d(); int Op6e(); int Op6f();
    int Op70(); int Op71(); int Op72(); int Op73(); int Op74(); int Op75(); int Op76(); int Op77();
    int Op78(); int Op79(); int Op7a(); int Op7b(); int Op7c(); int Op7d(); int Op7e(); int Op7f();
    int Op80(); int Op81(); int Op82(); int Op83(); int Op84(); int Op85(); int Op86(); int Op87();
    int Op88(); int Op89(); int Op8a(); int Op8b(); int Op8c(); int Op8d(); int Op8e(); int Op8f();
    int Op90(); int Op91(); int Op92(); int Op93(); int Op94(); int Op95(); int Op96(); int Op97();
    int Op98(); int Op99(); int Op9a(); int Op9b(); int Op9c(); int Op9d(); int Op9e(); int Op9f();
    int Opa0(); int Opa1(); int Opa2(); int Opa3(); int Opa4(); int Opa5(); int Opa6(); int Opa7();
    int Opa8(); int Opa9(); int Opaa(); int Opab(); int Opac(); int Opad(); int Opae(); int Opaf();
    int Opb0(); int Opb1(); int Opb2(); int Opb3(); int Opb4(); int Opb5(); int Opb6(); int Opb7();
    int Opb8(); int Opb9(); int Opba(); int Opbb(); int Opbc(); int Opbd(); int Opbe(); int Opbf();
    int Opc0(); int Opc1(); int Opc2(); int Opc3(); int Opc4(); int Opc5(); int Opc6(); int Opc7();
    int Opc8(); int Opc9(); int Opca(); int Opcb(); int Opcc(); int Opcd(); int Opce(); int Opcf();
    int Opd0(); int Opd1(); int Opd2(); int Opd3(); int Opd4(); int Opd5(); int Opd6(); int Opd7();
    int Opd8(); int Opd9(); int Opda(); int Opdb(); int Opdc(); int Opdd(); int Opde(); int Opdf();
    int Ope0(); int Ope1(); int Ope2(); int Ope3(); int Ope4(); int Ope5(); int Ope6(); int Ope7();
    int Ope8(); int Ope9(); int Opea(); int Opeb(); int Opec(); int Oped(); int Opee(); int Opef();
    int Opf0(); int Opf1(); int Opf2(); int Opf3(); int Opf4(); int Opf5(); int Opf6(); int Opf7();
    int Opf8(); int Opf9(); int Opfa(); int Opfb(); int Opfc(); int Opfd(); int Opfe(); int Opff();
};

}  // namespace chico

#endif  // CHICO_CPU_H
