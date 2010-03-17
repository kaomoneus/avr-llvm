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
#include "llvm/Support/ErrorHandling.h"
//#include <cstdio>
//#include "llvm/CodeGen/PseudoSourceValue.h"
//#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

AVRInstrInfo::AVRInstrInfo(AVRSubtarget &ST)
  : TargetInstrInfoImpl(AVRInsts, array_lengthof(AVRInsts)),
    RegInfo(ST, *this), Subtarget(ST) 
{

}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  
/// If not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned AVRInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                            int &FrameIndex) const
{
#if 0 
~~~STUB FROM PIC16~~~~
  if (MI->getOpcode() == AVR::movwf 
      && MI->getOperand(0).isReg()
      && MI->getOperand(1).isSymbol()) {
    FrameIndex = MI->getOperand(1).getIndex();
    return MI->getOperand(0).getReg();
  }
#endif
  llvm_unreachable("isStoreToStackSlot not yet implemented");
  return 0;
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the dest reg along with the FrameIndex of the stack slot.  
/// If not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned AVRInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
                                            int &FrameIndex) const
{
#if 1
  switch (MI->getOpcode())
  {
    default: break;
    case AVR::LDy:
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    /*
      if (MI->getOperand(1).isFI() &&
          MI->getOperand(2).isReg() &&
          MI->getOperand(3).isImm() &&
          MI->getOperand(2).getReg() == 0 &&
          MI->getOperand(3).getImm() == 0) {
        FrameIndex = MI->getOperand(1).getIndex();
        return MI->getOperand(0).getReg();
      }*/
      break;
  }
#endif
  //llvm_unreachable("isLoadFromStackSlot not yet implemented");
  return 0;
}


void AVRInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, 
                                         MachineBasicBlock::iterator MI,
                                         unsigned SrcReg, bool isKill, int FrameIdx,
                                         const TargetRegisterClass *RC) const
{
  DebugLoc dl = DebugLoc::getUnknownLoc();
  BuildMI(MBB, MI, dl, get(AVR::STy)).addFrameIndex(FrameIdx);

/*
  BuildMI(MBB, MI, dl, get(AVR::STy)).addReg(SrcReg,getKillRegState(isKill))
    .addImm(0).addFrameIndex(FrameIdx);*/
  //llvm_unreachable("Can't store this register to stack slot");

}

void AVRInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, 
                                          MachineBasicBlock::iterator MI,
                                          unsigned DestReg, int FrameIdx,
                                          const TargetRegisterClass *RC) const
{
#if 0
  DebugLoc DL = DebugLoc::getUnknownLoc();
  if (MI != MBB.end()) DL = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();

  MachineMemOperand *MMO =
    MF.getMachineMemOperand(PseudoSourceValue::getFixedStack(FrameIdx),
                            MachineMemOperand::MOLoad, 0,
                            MFI.getObjectSize(FrameIdx),
                            MFI.getObjectAlignment(FrameIdx));

  if (RC == &AVR::GPRegsRegClass)
    BuildMI(MBB, MI, DL, get(AVR::MOV))
      .addReg(DestReg).addFrameIndex(FrameIdx).addImm(0).addMemOperand(MMO);
  else
    llvm_unreachable("Cannot store this register to stack slot!");
#endif
    llvm_unreachable("Can't load this register from stack slot");

}

bool AVRInstrInfo::copyRegToReg (MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I,
                                   unsigned DestReg, unsigned SrcReg,
                                   const TargetRegisterClass *DestRC,
                                   const TargetRegisterClass *SrcRC) const 
{
  DebugLoc DL = DebugLoc::getUnknownLoc();
/*
  if (DestRC != SrcRC)
  {
    llvm_unreachable(DestRC->getName());
    return false;
  }  
*/
  if (DestRC == AVR::LDIRegsRegisterClass)
  {
    BuildMI(MBB, I, DL, get(AVR::LDI), DestReg).addReg(SrcReg);
    return true;
  }
  
  if (DestRC == AVR::GPRegsRegisterClass)
  {
    BuildMI(MBB, I, DL, get(AVR::MOV), DestReg).addReg(SrcReg);
    return true;
  }

  if (DestRC == AVR::WRegsRegisterClass)
  {
    BuildMI(MBB, I, DL, get(AVR::MOVW), DestReg).addReg(SrcReg);
    return true;
  }

  // Not yet supported.
  llvm_unreachable("copyRegToReg");
  return false;
}

bool AVRInstrInfo::isMoveInstr(const MachineInstr &MI,
                                 unsigned &SrcReg, unsigned &DstReg,
                                 unsigned &SrcSubIdx, unsigned &DstSubIdx) const 
{
  SrcSubIdx = DstSubIdx = 0; // No sub-registers.
  
  switch (MI.getOpcode())
  {
    default:
      return false;
    case AVR::MOV:
    case AVR::MOVW:
      assert(MI.getNumOperands() >= 2 &&
              MI.getOperand(0).isReg() &&
              MI.getOperand(1).isReg() &&
              "invalid register-register move instruction");
      SrcReg = MI.getOperand(1).getReg();
      DstReg = MI.getOperand(0).getReg();
      return true;
  }
}

void AVRInstrInfo::insertNoop(MachineBasicBlock &MBB, 
                                MachineBasicBlock::iterator MI) const 
{
  DebugLoc DL = DebugLoc::getUnknownLoc();
  if (MI != MBB.end()) DL = MI->getDebugLoc();
  BuildMI(MBB, MI, DL, get(AVR::NOP));
}
