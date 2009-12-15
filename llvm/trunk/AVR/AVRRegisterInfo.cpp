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
#include "AVRSubtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineLocation.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Type.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
using namespace llvm;

AVRRegisterInfo::AVRRegisterInfo(AVRSubtarget &st,
                                     const TargetInstrInfo &tii)
  : AVRGenRegisterInfo(AVR::ADJCALLSTACKDOWN, AVR::ADJCALLSTACKUP),
    Subtarget(st), TII(tii) {}

const unsigned* AVRRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  static const unsigned CalleeSavedRegs[] = {
    AVR::R0, AVR::R1,
    AVR::R2, AVR::R3, AVR::R4, AVR::R5, AVR::R6, AVR::R7, AVR::R8,
    AVR::R9, AVR::R10, AVR::R11, AVR::R12, AVR::R13, AVR::R14, AVR::R15,
    AVR::R16, AVR::R17,
    AVR::R28, AVR::R29,0};

  return CalleeSavedRegs;
}

BitVector AVRRegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());
  Reserved.set(AVR::R0);
  Reserved.set(AVR::R1);
  return Reserved;
}

const TargetRegisterClass* const*
AVRRegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const
{
  static const TargetRegisterClass * const CalleeSavedRegClasses[] = { &AVR::GPRegsRegClass, 0 };
  return CalleeSavedRegClasses;
}

bool AVRRegisterInfo::hasFP(const MachineFunction &MF) const
{
  return false;
}

void AVRRegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const {}

void AVRRegisterInfo::emitPrologue(MachineFunction &MF) const
{

}

void AVRRegisterInfo::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const
{

}

unsigned AVRRegisterInfo::getRARegister() const
{
  llvm_unreachable("What is the return address register");
  return AVR::SP;
}

unsigned AVRRegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  llvm_unreachable("What is the frame register");
  return AVR::Y_PTR;
}

unsigned AVRRegisterInfo::getEHExceptionRegister() const
{
  llvm_unreachable("What is the exception register");
  return 0;
}

unsigned AVRRegisterInfo::getEHHandlerRegister() const
{
  llvm_unreachable("What is the exception handler register");
  return 0;
}

int AVRRegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const
{
  return AVRGenRegisterInfo::getDwarfRegNumFull(RegNum, 0);
}

#include "AVRGenRegisterInfo.inc"
