//===----- AVRRegisterInfo.h - AVR Register Information Impl ----*- C++ -*-===//
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

#ifndef __INCLUDE_AVRREGISTERINFO_H__
#define __INCLUDE_AVRREGISTERINFO_H__

#include "llvm/Target/TargetRegisterInfo.h"
#include "AVRGenRegisterInfo.h.inc"

namespace llvm
{

class AVRTargetMachine;
class TargetInstrInfo;

class AVRRegisterInfo : public AVRGenRegisterInfo
{
public:
  AVRRegisterInfo(const TargetInstrInfo &tii);
public:
  static unsigned getAVRRegNum(unsigned reg);
public: // TargetRegisterInfo
  const unsigned *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
  BitVector getReservedRegs(const MachineFunction &MF) const;
  void eliminateFrameIndex(MachineBasicBlock::iterator MI,
                           int SPAdj,
                           RegScavenger *RS = NULL) const;
  int getDwarfRegNum(unsigned RegNum, bool isEH) const;
  int getLLVMRegNum(unsigned RegNum, bool isEH) const;
  unsigned getFrameRegister(const MachineFunction &MF) const;
  unsigned getRARegister() const;
private:
  const TargetInstrInfo &TII; //:TODO: is this needed?!
};

} // end llvm namespace

#endif //__INCLUDE_AVRREGISTERINFO_H__
