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

  //:TODO: someday convert this to use regmasks
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
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
  const TargetRegisterInfo *TRI = MF.getTarget().getRegisterInfo();
  DebugLoc dl = MI->getDebugLoc();
  int Opcode = MI->getOpcode();
  uint64_t Amount = MI->getOperand(0).getImm();

  // Turn the adjcallstackup instruction into a 'sbiw reg, <amt>' and the
  // adjcallstackdown instruction into 'adiw reg, <amt>' also handle reading
  // and writing SP from I/O space.
  if (Amount != 0)
  {
    // We need to keep the stack aligned properly.  To do this, we round the
    // amount of space needed for the outgoing arguments up to the next
    // alignment boundary.
    unsigned Align = TFI->getStackAlignment();
    Amount = (Amount + Align - 1) / Align * Align;

    if (Opcode == TII.getCallFrameSetupOpcode())
    {
      // The next instruction after adjcallstackdown is a SPLOAD, first get the
      // register it defines and then move the instruction to the start of
      // sequence we are about to generate.
      MachineBasicBlock::iterator SPLOAD = llvm::next(MI);
      assert(SPLOAD->getOpcode() == AVR::SPLOAD
             && "Expected a SPLOAD instruction");
      unsigned DstReg = SPLOAD->getOperand(0).getReg();
      MBB.erase(SPLOAD);
      BuildMI(MBB, MI, dl, TII.get(AVR::SPLOAD), DstReg).addReg(AVR::SP);

      // Select optimal opcode to adjust SP based on DstReg and the offset size
      unsigned subOpcode;
      switch (DstReg)
      {
      case AVR::R25R24:
      case AVR::R27R26:
      case AVR::R29R28:
      case AVR::R31R30:
        {
          if (Amount < 64)
          {
            subOpcode = AVR::SBIWRdK;
            break;
          }
          // Fallthru
        }
      default:
        {
          subOpcode = AVR::SUBIWRdK;
          break;
        }
      }

      // Generate the real code that reads, modifies and then writes SP back
      // to I/O space disabling temporarily interrupts.
      MachineInstr *New = BuildMI(MBB, MI, dl, TII.get(subOpcode), DstReg)
                            .addReg(DstReg, RegState::Kill)
                            .addImm(Amount);
      New->getOperand(3).setIsDead();
      BuildMI(MBB, MI, dl, TII.get(AVR::INRdA), AVR::R0)
        .addImm(0x3f);
      BuildMI(MBB, MI, dl, TII.get(AVR::CLI));
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3e)
        .addReg(TRI->getSubReg(DstReg, AVR::sub_hi));
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3f)
        .addReg(AVR::R0, RegState::Kill);
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3d)
        .addReg(TRI->getSubReg(DstReg, AVR::sub_lo));
    }
    else
    {
      assert(Opcode == TII.getCallFrameDestroyOpcode());
      // Select optimal opcode to adjust SP based on DstReg and the offset size
      unsigned addOpcode;
      unsigned DstReg = MI->getOperand(1).getReg();
      bool DstIsKill = MI->getOperand(1).isKill();
      switch (DstReg)
      {
      case AVR::R25R24:
      case AVR::R27R26:
      case AVR::R29R28:
      case AVR::R31R30:
        {
          if (Amount < 64)
          {
            addOpcode = AVR::ADIWRdK;
            break;
          }
          // Fallthru
        }
      default:
        {
          addOpcode = AVR::SUBIWRdK;
          Amount = -Amount;
          break;
        }
      }

      MachineInstr *New = BuildMI(MBB, MI, dl, TII.get(addOpcode), DstReg)
                            .addReg(DstReg, RegState::Kill)
                            .addImm(Amount);
      New->getOperand(3).setIsDead();
      BuildMI(MBB, MI, dl, TII.get(AVR::INRdA), AVR::R0)
        .addImm(0x3f);
      BuildMI(MBB, MI, dl, TII.get(AVR::CLI));
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3e)
        .addReg(TRI->getSubReg(DstReg, AVR::sub_hi),
                getKillRegState(DstIsKill));
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3f)
        .addReg(AVR::R0, RegState::Kill);
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3d)
        .addReg(TRI->getSubReg(DstReg, AVR::sub_lo),
                getKillRegState(DstIsKill));
      }
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
        break;
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
