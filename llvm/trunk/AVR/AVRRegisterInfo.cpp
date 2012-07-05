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

#include "AVRRegisterInfo.h"
#include "AVR.h"
#include "AVRInstrInfo.h"
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

  Reserved.set(AVR::SPL);
  Reserved.set(AVR::SPH);
  Reserved.set(AVR::SP);

  if (TFI->hasFP(MF))
  {
    Reserved.set(AVR::R28);
    Reserved.set(AVR::R29);
    Reserved.set(AVR::R29R28);
  }

  return Reserved;
}

/// Replace pseudo store instructions that pass arguments through the stack with
/// real instructions. If insertPushes is true then all instructions are
/// replaced with push instructions, otherwise regular std instructions are
/// inserted.
static void fixStackStores(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI,
                           const TargetInstrInfo &TII, bool insertPushes)
{
  // Iterate through the BB until we hit a call instruction or we reach the end.
  for (MachineBasicBlock::iterator I = MI, E = MBB.end();
        (I != E) && (!I->isCall()); )
  {
    MachineBasicBlock::iterator NextMI = llvm::next(I);
    MachineInstr &MI = *I;
    int Opcode = I->getOpcode();

    // Only care of pseudo store instructions where SP is the base pointer.
    if ((Opcode != AVR::STDSPQRr) && (Opcode != AVR::STDWSPQRr))
    {
      I = NextMI;
      continue;
    }

    unsigned SrcReg = MI.getOperand(2).getReg();
    bool SrcIsKill = MI.getOperand(2).isKill();
    assert(MI.getOperand(0).getReg() == AVR::SP
           && "Invalid register, should be SP!");

    if (insertPushes)
    {
      // Replace this instruction with a push.

      // We can't use PUSHWRr here because when expanded the order of the new
      // instructions are reversed from what we need. Perform the expansion now.
      if (Opcode == AVR::STDWSPQRr)
      {
        const TargetRegisterInfo *TRI =
          MBB.getParent()->getTarget().getRegisterInfo();

        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
          .addReg(TRI->getSubReg(SrcReg, AVR::sub_hi),
                  getKillRegState(SrcIsKill));
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
          .addReg(TRI->getSubReg(SrcReg, AVR::sub_lo),
                  getKillRegState(SrcIsKill));
      }
      else
      {
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      }
    }
    else
    {
      // Replace this instruction with a regular store. Use Y as the base
      // pointer because it is guaranteed to contain a copy of SP.
      int STOpc = (Opcode == AVR::STDWSPQRr) ? AVR::STDWPtrQRr : AVR::STDPtrQRr;
      unsigned Imm = MI.getOperand(1).getImm();
      assert(isUInt<6>(Imm) && "Offset is out of range");

      MachineInstrBuilder MIB =
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(STOpc))
          .addReg(AVR::R29R28)
          .addImm(Imm)
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      MIB->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
    }

    MI.eraseFromParent();
    I = NextMI;
  }
}

void AVRRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const
{
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  // There is nothing to insert when the call frame memory is allocated during
  // function entry. Delete the call frame pseudo and replace all pseudo stores
  // with real store instructions.
  if (TFI->hasReservedCallFrame(MF))
  {
    fixStackStores(MBB, MI, TII, false);
    MBB.erase(MI);
    return;
  }

  DebugLoc dl = MI->getDebugLoc();
  int Opcode = MI->getOpcode();
  int Amount = MI->getOperand(0).getImm();

  // Adjcallstackup does not need to allocate stack space for the call, instead
  // we insert push instructions that will allocate the necessary stack.
  // For adjcallstackdown we convert it into an 'adiw reg, <amt>' handling
  // reading and writing SP from I/O space.
  if (Amount != 0)
  {
    assert(TFI->getStackAlignment() == 1 && "Unsupported stack alignment");

    if (Opcode == TII.getCallFrameSetupOpcode())
    {
      fixStackStores(MBB, MI, TII, true);
    }
    else
    {
      assert(Opcode == TII.getCallFrameDestroyOpcode());

      // Select the best opcode to adjust SP based on the offset size.
      int addOpcode;
      if (isUInt<6>(Amount))
      {
        addOpcode = AVR::ADIWRdK;
      }
      else
      {
        addOpcode = AVR::SUBIWRdK;
        Amount = -Amount;
      }

      // Build the instruction sequence.
      BuildMI(MBB, MI, dl, TII.get(AVR::INWRdA), AVR::R31R30).addImm(0x3d);
      MachineInstr *New = BuildMI(MBB, MI, dl, TII.get(addOpcode), AVR::R31R30)
                            .addReg(AVR::R31R30, RegState::Kill)
                            .addImm(Amount);
      New->getOperand(3).setIsDead();
      BuildMI(MBB, MI, dl, TII.get(AVR::INRdA), AVR::R0)
        .addImm(0x3f);
      BuildMI(MBB, MI, dl, TII.get(AVR::CLI));
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3e)
        .addReg(AVR::R31, RegState::Kill);
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3f)
        .addReg(AVR::R0, RegState::Kill);
      BuildMI(MBB, MI, dl, TII.get(AVR::OUTARr))
        .addImm(0x3d)
        .addReg(AVR::R30, RegState::Kill);
      }
  }

  MBB.erase(MI);
}

/// Fold a frame offset shared between two add instructions into a single one.
static void foldFrameOffset(MachineInstr &MI, int &Offset, unsigned DstReg)
{
  int Opcode = MI.getOpcode();

  // Don't bother trying if the next instruction is not an add or a sub
  if ((Opcode != AVR::SUBIWRdK) && (Opcode != AVR::ADIWRdK))
  {
    return;
  }

  // Check that DstReg matches with next instruction, otherwise the instruction
  // is not related to stack address manipulation.
  if (DstReg != MI.getOperand(0).getReg())
  {
    return;
  }

  // Add the offset in the next instruction to our offset.
  switch (Opcode)
  {
  case AVR::SUBIWRdK:
    Offset += -MI.getOperand(2).getImm();
    break;
  case AVR::ADIWRdK:
    Offset += MI.getOperand(2).getImm();
    break;
  }

  // Finally remove the instruction
  MI.eraseFromParent();
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
    int Opcode;
    unsigned DstReg = MI.getOperand(0).getReg();

    // Generally, to load a frame address two add instructions get emitted that
    // could be folded into a single one like this:
    //  movw    r31:r30, r29:r28
    //  adiw    r31:r30, 29
    //  adiw    r31:r30, 16
    // to:
    //  movw    r31:r30, r29:r28
    //  adiw    r31:r30, 45
    foldFrameOffset(*llvm::next(II), Offset, DstReg);

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
