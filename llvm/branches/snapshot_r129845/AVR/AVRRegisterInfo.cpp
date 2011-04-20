//===- AVRRegisterInfo.cpp - AVR Register Information -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the AVR implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "AVR.h"
#include "AVRRegisterInfo.h"
#include "llvm/ADT/BitVector.h"

using namespace llvm;

AVRRegisterInfo::AVRRegisterInfo(const TargetInstrInfo &tii) :
  AVRGenRegisterInfo(AVR::ADJCALLSTACKDOWN, AVR::ADJCALLSTACKUP), TII(tii) {}

unsigned AVRRegisterInfo::getAVRRegNum(unsigned reg)
{
  switch (reg)
  {
  case AVR::R0:               return 0;
  case AVR::R1:               return 1;
  case AVR::R2:               return 2;
  case AVR::R3:               return 3;
  case AVR::R4:               return 4;
  case AVR::R5:               return 5;
  case AVR::R6:               return 6;
  case AVR::R7:               return 7;
  case AVR::R8:               return 8;
  case AVR::R9:               return 9;
  case AVR::R10:              return 10;
  case AVR::R11:              return 11;
  case AVR::R12:              return 12;
  case AVR::R13:              return 13;
  case AVR::R14:              return 14;
  case AVR::R15:              return 15;
  case AVR::R16:              return 16;
  case AVR::R17:              return 17;
  case AVR::R18:              return 18;
  case AVR::R19:              return 19;
  case AVR::R20:              return 20;
  case AVR::R21:              return 21;
  case AVR::R22:              return 22;
  case AVR::R23:              return 23;
  case AVR::R24:              return 24;
  case AVR::R25:              return 25;
  case AVR::R26:              return 26;
  case AVR::R27:              return 27;
  case AVR::R28:              return 28;
  case AVR::R29:              return 29;
  case AVR::R30:              return 30;
  case AVR::R31:              return 31;
  default:
    {
      assert(isVirtualRegister(reg) && "Unknown physical register!");
      return 0;
    }
  }
}

const unsigned *
AVRRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  static const unsigned CalleeSavedRegs[] =
  {
    /*AVR::R28, AVR::R29, AVR::R17, AVR::R16, AVR::R15, AVR::R14, AVR::R13,
    AVR::R12, AVR::R11, AVR::R10, AVR::R9, AVR::R8, AVR::R7, AVR::R6,
    AVR::R5, AVR::R4, AVR::R3, AVR::R2, AVR::R0, AVR::R1, */NULL
  };

  return CalleeSavedRegs;
}

BitVector AVRRegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());
  //Reserved.set(AVR::R0);
  //Reserved.set(AVR::R1);

  return Reserved;
}

void AVRRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                              int SPAdj, RegScavenger *RS) const
{
  return;
}

int AVRRegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const
{
  return AVRGenRegisterInfo::getDwarfRegNumFull(RegNum, 0);
}

unsigned AVRRegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  return 28;
}

unsigned AVRRegisterInfo::getRARegister() const
{
  //llvm_unreachable("AVR Does not have any return address register");
  //:TODO:
  return 0;
}

#include "AVRGenRegisterInfo.inc"
