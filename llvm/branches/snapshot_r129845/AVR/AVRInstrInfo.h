//===------- AVRInstrInfo.h - AVR Instruction Information ------*- C++ -*- ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the AVR implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef __INCLUDE_AVRINSTRINFO_H__
#define __INCLUDE_AVRINSTRINFO_H__

#include "AVRRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

namespace llvm
{

class AVRInstrInfo : public TargetInstrInfoImpl
{
public:
  AVRInstrInfo();
  const AVRRegisterInfo &getRegisterInfo() const;
public: // TargetInstrInfo
  void copyPhysReg(MachineBasicBlock &MBB,
                   MachineBasicBlock::iterator MI,
                   DebugLoc DL,
                   unsigned DestReg,
                   unsigned SrcReg,
                   bool KillSrc) const;
private:
  const AVRRegisterInfo RI;
};

} // end llvm namespace

#endif //__INCLUDE_AVRINSTRINFO_H__
