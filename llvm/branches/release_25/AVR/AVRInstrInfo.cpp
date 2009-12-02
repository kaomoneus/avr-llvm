//===- AVRInstrInfo.cpp - AVR Instruction Information ---------------------===//
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
#include "AVRTargetMachine.h"
#include "AVRGenInstrInfo.inc"
#include "AVRSubtarget.h"
#include "llvm/Function.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include <cstdio>


using namespace llvm;

// FIXME: Add the subtarget support on this constructor.
AVRInstrInfo::AVRInstrInfo(AVRSubtarget &ST)
  : TargetInstrInfoImpl(AVRInsts, array_lengthof(AVRInsts)),
    RegInfo(ST, *this), Subtarget(ST) {}


/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  
/// If not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned AVRInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                            int &FrameIndex) const {
/*  if (MI->getOpcode() == AVR::movwf 
      && MI->getOperand(0).isReg()
      && MI->getOperand(1).isSymbol()) {
    FrameIndex = MI->getOperand(1).getIndex();
    return MI->getOperand(0).getReg();
  }*/
  assert(0 && __THIS_IS_A_TEMPORARY_MACRO__);
  return 0;
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the dest reg along with the FrameIndex of the stack slot.  
/// If not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned AVRInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                            int &FrameIndex) const {
/*  if (MI->getOpcode() == AVR::movf 
      && MI->getOperand(0).isReg()
      && MI->getOperand(1).isSymbol()) {
    FrameIndex = MI->getOperand(1).getIndex();
    return MI->getOperand(0).getReg();
  }*/
  assert(0 && __THIS_IS_A_TEMPORARY_MACRO__);
  return 0;
}


void AVRInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, 
                                         MachineBasicBlock::iterator I,
                                         unsigned SrcReg, bool isKill, int FI,
                                         const TargetRegisterClass *RC) const {
/*
  const Function *Func = MBB.getParent()->getFunction();
  const std::string FuncName = Func->getName();

  char *tmpName = new char [strlen(FuncName.c_str()) +  6];
  sprintf(tmpName, "%s.tmp", FuncName.c_str());

  // On the order of operands here: think "movwf SrcReg, tmp_slot, offset".
  if (RC == AVR::GPRRegisterClass) {
    //MachineFunction &MF = *MBB.getParent();
    //MachineRegisterInfo &RI = MF.getRegInfo();
    BuildMI(MBB, I, get(AVR::movwf))
      .addReg(SrcReg, false, false, isKill)
      .addImm(FI)
      .addExternalSymbol(tmpName)
      .addImm(1); // Emit banksel for it.
  }
  else if (RC == AVR::FSR16RegisterClass)
    assert(0 && "Don't know yet how to store a FSR16 to stack slot");
  else*/
    assert(0 && "Can't store this register to stack slot");

}

void AVRInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, 
                                          MachineBasicBlock::iterator I,
                                          unsigned DestReg, int FI,
                                          const TargetRegisterClass *RC) const {
/*
  const Function *Func = MBB.getParent()->getFunction();
  const std::string FuncName = Func->getName();

  char *tmpName = new char [strlen(FuncName.c_str()) +  6];
  sprintf(tmpName, "%s.tmp", FuncName.c_str());

  // On the order of operands here: think "movf FrameIndex, W".
  if (RC == AVR::GPRRegisterClass) {
    //MachineFunction &MF = *MBB.getParent();
    //MachineRegisterInfo &RI = MF.getRegInfo();
    BuildMI(MBB, I, get(AVR::movf), DestReg)
      .addImm(FI)
      .addExternalSymbol(tmpName)
      .addImm(1); // Emit banksel for it.
  }
  else if (RC == AVR::FSR16RegisterClass)
    assert(0 && "Don't know yet how to load an FSR16 from stack slot");
  else*/
    assert(0 && "Can't load this register from stack slot");

}

bool AVRInstrInfo::copyRegToReg (MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I,
                                   unsigned DestReg, unsigned SrcReg,
                                   const TargetRegisterClass *DestRC,
                                   const TargetRegisterClass *SrcRC) const {
  
  if (DestRC != SrcRC) {
    // Not yet supported
    return false;
  }  

  if (DestRC == AVR::GPRegsRegisterClass) {
    BuildMI(MBB, I, get(AVR::MOV), DestReg).addReg(SrcReg);
    return true;
  }

  if (DestRC == AVR::WRegsRegisterClass) {
    BuildMI(MBB, I, get(AVR::MOVW), DestReg).addReg(SrcReg);
    return true;
  }

  // Not yet supported.
  return false;
}

bool AVRInstrInfo::isMoveInstr(const MachineInstr &MI,
                                 unsigned &SrcReg, unsigned &DestReg,
                                 unsigned &SrcSubIdx, unsigned &DstSubIdx) const {
/*  SrcSubIdx = DstSubIdx = 0; // No sub-registers.

  if (MI.getOpcode() == AVR::copy_fsr
      || MI.getOpcode() == AVR::copy_w) {
    DestReg = MI.getOperand(0).getReg();
    SrcReg = MI.getOperand(1).getReg();
    return true;
  }
*/
  return false;
}

