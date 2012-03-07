//===-- AVRRegisterInfo.cpp - AVR Register Information --------------------===//
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
#include "AVRInstrInfo.h"
#include "AVRRegisterInfo.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/ADT/BitVector.h"

#define GET_REGINFO_TARGET_DESC
#include "AVRGenRegisterInfo.inc"

using namespace llvm;

AVRRegisterInfo::AVRRegisterInfo(const TargetInstrInfo &tii) :
  AVRGenRegisterInfo(0),
  TII(tii) {}

const uint16_t *
AVRRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  const Function *F = MF->getFunction();

  //:TODO: test if adding the 8bit regs here, makes 8bit pushes in
  // prologue code instead of pushing the whole pair
  // IT WORKS! so add all the 8bit regs here!
  static const uint16_t CalleeSavedRegs[] =
  {
    AVR::R29R28, AVR::R17R16, AVR::R15R14, AVR::R13R12, AVR::R11R10,
    AVR::R9R8, AVR::R7R6, AVR::R5R4, AVR::R3R2, AVR::R1R0, 0
  };
  static const uint16_t IntCalleeSavedRegs[] =
  {
    AVR::R31R30, AVR::R29R28, AVR::R27R26, AVR::R25R24, AVR::R23R22,
    AVR::R21R20, AVR::R19R18, AVR::R17R16, AVR::R15R14, AVR::R13R12,
    AVR::R11R10, AVR::R9R8, AVR::R7R6, AVR::R5R4, AVR::R3R2, AVR::R1R0, 0
  };

  return ((F->getCallingConv() == CallingConv::AVR_INTR
           || F->getCallingConv() == CallingConv::AVR_SIGNAL) ?
          IntCalleeSavedRegs : CalleeSavedRegs);
}

BitVector AVRRegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  Reserved.set(AVR::R0);
  Reserved.set(AVR::R1);
  Reserved.set(AVR::R1R0);

  if (TFI->hasFP(MF))
  {
    Reserved.set(AVR::R28);
    Reserved.set(AVR::R29);
    Reserved.set(AVR::R29R28);
  }

  return Reserved;
}

void AVRRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const
{
  //:TODO: implement proper stack manipulation when we get IO and interrupt
  // instrs. This code is just a place holder.
    MachineInstr *Old = MI;
  MachineInstr *New = 0;
  uint64_t Amount = Old->getOperand(0).getImm();
  if (!Amount) { MBB.erase(MI); return;}

      if (Old->getOpcode() == TII.getCallFrameSetupOpcode()) {
        New = BuildMI(MF, Old->getDebugLoc(),
                      TII.get(AVR::SBIWRdK), AVR::R29R28)
          .addReg(AVR::R29R28).addImm(Amount);
      } else {
         New = BuildMI(MF, Old->getDebugLoc(),
                        TII.get(AVR::ADIWRdK), AVR::R29R28)
            .addReg(AVR::R29R28).addImm(Amount);
      }
      if (New) {
        // The SRW implicit def is dead.
        //New->getOperand(3).setIsDead();

        // Replace the pseudo instruction with a new instruction...
        MBB.insert(MI, New);
      }

  MBB.erase(MI);
}

void AVRRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, RegScavenger *RS) const
{
  assert(SPAdj == 0 && "Unexpected SPAdj value");

  unsigned i = 0;
  MachineInstr &MI = *II;
  DebugLoc dl = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  const MachineFunction &MF = *MBB.getParent();
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  while (!MI.getOperand(i).isFI())
  {
    ++i;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();
  int Offset = MFI->getObjectOffset(FrameIndex);

  // Add one to the offset because SP points to an empty slot
  Offset += MFI->getStackSize() - TFI->getOffsetOfLocalArea() + 1;
  // Fold incoming offset
  Offset += MI.getOperand(i + 1).getImm();

  // This is actually "load effective address" of the stack slot
  // instruction. We have only two-address instructions, thus we need to
  // expand it into move + add.
  if (MI.getOpcode() == AVR::FRMIDX)
  {
    MI.setDesc(TII.get(AVR::MOVWRdRr));
    MI.getOperand(i).ChangeToRegister(AVR::R29R28, false);

    assert(Offset > 0 && "Invalid offset");

    // We need to materialize the offset via an add instruction.
    unsigned Opcode;
    unsigned DstReg = MI.getOperand(0).getReg();

    // Select the optimal opcode based on DstReg and the offset size
    switch (DstReg)
    {
    case AVR::R25R24:
    case AVR::R27R26:
    case AVR::R31R30:
      {
        if (Offset < 64)
        {
          Opcode = AVR::ADIWRdK;
          break;
        }
        // Fallthru
      }
    default:
      {
        // This opcode will get expanded into a pair of subi/sbci
        //:FIXME: INVALID CODEGEN: NON IMM REGS CANT USE SUBI!!!
        // we have to materialize the offset by other means, see what gcc does
        // For now, FRMIDX only accepts DLDREGS which looks like a good solution
        // but check if it's the best thing to do.
        Opcode = AVR::SUBIWRdK;
        Offset = -Offset;
      }
    }

    BuildMI(MBB, llvm::next(II), dl, TII.get(Opcode), DstReg).addReg(DstReg)
      .addImm(Offset);

    return;
  }

  // if the offset is too big we have to adjust and restore the frame pointer
  // to materialize a valid load/store with displacement
  //:TODO: consider using only one adiw/sbiw chain for more than one frame index
  if (Offset >= 63)
  {
    //assert((Offset - 63 + 1) < 63 && "Implement subi/sbci for huge offsets");

    //:FIXME: use subi/sbci when Offset - 63 + 1 > 63
    BuildMI(MBB, II, dl, TII.get(AVR::ADIWRdK), AVR::R29R28)
      .addReg(AVR::R29R28).addImm(Offset - 63 + 1);
    BuildMI(MBB, llvm::next(II), dl, TII.get(AVR::SBIWRdK), AVR::R29R28)
      .addReg(AVR::R29R28).addImm(Offset - 63 + 1);

    Offset = 62;
  }

  MI.getOperand(i).ChangeToRegister(AVR::R29R28, false);
  MI.getOperand(i + 1).ChangeToImmediate(Offset);
}

unsigned AVRRegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  assert(0 && "implement getFrameRegister");

  return 28;
}
