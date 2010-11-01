//===------ AVRInstrInfo.cpp - AVR Instruction Information ------*- C++ -*-===//
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

#include "AVR.h"
#include "AVRInstrInfo.h"
#include "AVRGenInstrInfo.inc"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

AVRInstrInfo::AVRInstrInfo() :
  TargetInstrInfoImpl(AVRInsts, array_lengthof(AVRInsts)), RI(*this) {}

const AVRRegisterInfo &AVRInstrInfo::getRegisterInfo() const
{
  return RI;
}

void AVRInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI, DebugLoc DL,
                               unsigned DestReg, unsigned SrcReg,
                               bool KillSrc) const
{
  unsigned Opc;

  if (AVR::GPR8RegClass.contains(DestReg, SrcReg)
      || AVR::LDREGSRegClass.contains(DestReg, SrcReg))
  {
    Opc = AVR::MOVRdRr;
  }
  else
  {
    llvm_unreachable("Impossible reg-to-reg copy");
  }

  BuildMI(MBB, MI, DL, get(Opc), DestReg).addReg(SrcReg,
                                                 getKillRegState(KillSrc));
}
