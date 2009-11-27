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
    Subtarget(st), TII(tii) {
}

const unsigned* AVRRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
                                                                         const {
  static const unsigned CalleeSavedRegs[] = {
    AVR::R0, AVR::R1,
    AVR::R2, AVR::R3, AVR::R4, AVR::R5, AVR::R6, AVR::R7, AVR::R8,
    AVR::R9, AVR::R10, AVR::R11, AVR::R12, AVR::R13, AVR::R14, AVR::R15,
    AVR::R16, AVR::R17,
    AVR::R28, AVR::R29};

  return CalleeSavedRegs;
}

BitVector AVRRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(AVR::R0);
  Reserved.set(AVR::R1);
  return Reserved;
}


const TargetRegisterClass* const*
AVRRegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const {
  static const TargetRegisterClass * const CalleeSavedRegClasses[] = { &AVR::GPRegsRegClass, 0 };
  return CalleeSavedRegClasses;
}

bool AVRRegisterInfo::hasFP(const MachineFunction &MF) const {
  return false;
}
/**/
void AVRRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  MachineInstr &MI = *I;
  int Size = MI.getOperand(0).getImm();
  if (MI.getOpcode() == AVR::ADJCALLSTACKDOWN)
    Size = -Size;
 /* if (Size)
    BuildMI(MBB, I, TII.get(AVR::ADDri), AVR::O6).addReg(AVR::O6).addImm(Size);*/
  MBB.erase(I);
}


// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
unsigned int AVRRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, 
                    int *Value, RegScavenger *RS) const 
{
  /*MachineInstr &MI    = *II;
  MachineFunction &MF = *MI.getParent()->getParent();

  unsigned i = 0;
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() && 
           "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();
  int stackSize  = MF.getFrameInfo()->getStackSize();
  int spOffset   = MF.getFrameInfo()->getObjectOffset(FrameIndex);

  DOUT << "\nFunction : " << MF.getFunction()->getName() << "\n";
  DOUT << "<--------->\n";
#ifndef NDEBUG
  MI.print(DOUT);
#endif
  DOUT << "FrameIndex : " << FrameIndex << "\n";
  DOUT << "spOffset   : " << spOffset << "\n";
  DOUT << "stackSize  : " << stackSize << "\n";

  // As explained on LowerFORMAL_ARGUMENTS, detect negative offsets 
  // and adjust SPOffsets considering the final stack size.
  int Offset = ((spOffset < 0) ? (stackSize + (-(spOffset+4))) : (spOffset));

  DOUT << "Offset     : " << Offset << "\n";
  DOUT << "<--------->\n";

  // MI.getOperand(i+1).ChangeToImmediate(Offset);
  MI.getOperand(i).ChangeToRegister(getFrameRegister(MF), false);*/
  assert(0 && "Not implemented");
  return 0;
}

void AVRRegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const {}
/**/
void AVRRegisterInfo::emitPrologue(MachineFunction &MF) const {
/*  MachineBasicBlock &MBB = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  // Get the number of bytes to allocate from the FrameInfo
  int NumBytes = (int) MFI->getStackSize();

  // Emit the correct save instruction based on the number of bytes in
  // the frame. Minimum stack frame size according to V8 ABI is:
  //   16 words for register window spill
  //    1 word for address of returned aggregate-value
  // +  6 words for passing parameters on the stack
  // ----------
  //   23 words * 4 bytes per word = 92 bytes
  NumBytes += 92;
  // Round up to next doubleword boundary -- a double-word boundary
  // is required by the ABI.
  NumBytes = (NumBytes + 7) & ~7;
  NumBytes = -NumBytes;
  
  if (NumBytes >= -4096) {
    BuildMI(MBB, MBB.begin(), TII.get(AVR::SAVEri),
            AVR::O6).addReg(AVR::O6).addImm(NumBytes);
  } else {
    MachineBasicBlock::iterator InsertPt = MBB.begin();
    // Emit this the hard way.  This clobbers G1 which we always know is 
    // available here.
    unsigned OffHi = (unsigned)NumBytes >> 10U;
    BuildMI(MBB, InsertPt, TII.get(AVR::SETHIi), AVR::G1).addImm(OffHi);
    // Emit G1 = G1 + I6
    BuildMI(MBB, InsertPt, TII.get(AVR::ORri), AVR::G1)
      .addReg(AVR::G1).addImm(NumBytes & ((1 << 10)-1));
    BuildMI(MBB, InsertPt, TII.get(AVR::SAVErr), AVR::O6)
      .addReg(AVR::O6).addReg(AVR::G1);
  }*/
}

void AVRRegisterInfo::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
}

unsigned AVRRegisterInfo::getRARegister() const {
  assert(0 && "What is the return address register");
  return 0;
}

unsigned AVRRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  assert(0 && "What is the frame register");
  return AVR::Y_PTR;
}

unsigned AVRRegisterInfo::getEHExceptionRegister() const {
  assert(0 && "What is the exception register");
  return 0;
}

unsigned AVRRegisterInfo::getEHHandlerRegister() const {
  assert(0 && "What is the exception handler register");
  return 0;
}

int AVRRegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const {
  assert(0 && "What is the dwarf register number");
  return -1;
}

#include "AVRGenRegisterInfo.inc"

