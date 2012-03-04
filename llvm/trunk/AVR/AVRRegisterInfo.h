//===-- AVRRegisterInfo.h - AVR Register Information Impl -------*- C++ -*-===//
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

#define GET_REGINFO_HEADER
#include "AVRGenRegisterInfo.inc"

namespace llvm
{

class TargetInstrInfo;

class AVRRegisterInfo : public AVRGenRegisterInfo
{
public:
  AVRRegisterInfo(const TargetInstrInfo &tii);
public: // TargetRegisterInfo
  /// Code Generation virtual methods...
  const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
  BitVector getReservedRegs(const MachineFunction &MF) const;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           RegScavenger *RS = NULL) const;
  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MI) const;

  /// Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const;
private:
  const TargetInstrInfo &TII;
};

} // end namespace llvm

#endif //__INCLUDE_AVRREGISTERINFO_H__
