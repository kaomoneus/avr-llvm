//===-- AVR16bitInstPass.cpp - Replace 8 bit moves with a movw --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "avr-16bitinst"

#include "AVR.h"
#include "AVRInstrInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

STATISTIC(NumMOVWS, "Number of movw instructions inserted");

namespace
{
  class AVR16bitInstPass : public MachineFunctionPass
  {
  public:
    static char ID;
    AVR16bitInstPass() : MachineFunctionPass(ID) {}

    bool runOnMachineFunction(MachineFunction &MF);

    const char *getPassName() const
    {
      return "AVR 16bit instruction inserter pass";
    }

  private:
    bool InsertMovws(MachineBasicBlock &MBB);
  private:
    const TargetInstrInfo *TII;
  };

  char AVR16bitInstPass::ID = 0;
}

bool AVR16bitInstPass::InsertMovws(MachineBasicBlock &MBB)
{
  bool Modified = false;

  for (MachineBasicBlock::iterator I = MBB.begin(), E = MBB.end(); I != E; )
  {
    // :NOTE: Currently we only support the following transformations:
    //   mov r10, r20
    //   mov r11, r21
    // into:
    //   movw r10, r20
    //
    // Or:
    //   mov r11, r21
    //   mov r10, r20
    // into:
    //   movw r10, r20

    MachineInstr *MI = I++;
    MachineInstr *NextMI = I;
    DebugLoc dl = MI->getDebugLoc();

        // only combine two moves in a row into a movw
    if ((MI->getOpcode() != AVR::MOVRdRr)
        || (NextMI->getOpcode() != AVR::MOVRdRr))
    {
      continue;
    }

    unsigned RegSrc_Lo = MI->getOperand(1).getReg();
    unsigned RegDst_Lo = MI->getOperand(0).getReg();
    unsigned RegSrc_Hi = NextMI->getOperand(1).getReg();
    unsigned RegDst_Hi = NextMI->getOperand(0).getReg();

    unsigned AVRRegSrc_Lo = AVRRegisterInfo::getAVRRegNum(RegSrc_Lo);
    unsigned AVRRegDst_Lo = AVRRegisterInfo::getAVRRegNum(RegDst_Lo);
    unsigned AVRRegSrc_Hi = AVRRegisterInfo::getAVRRegNum(RegSrc_Hi);
    unsigned AVRRegDst_Hi = AVRRegisterInfo::getAVRRegNum(RegDst_Hi);

    // first move must come either in even or odd registers
    // check if both moves use the same data block or if they're independent
    if (((AVRRegSrc_Lo & 1)
         || (AVRRegDst_Lo & 1)
         || (AVRRegSrc_Hi != AVRRegSrc_Lo + 1)
         || (AVRRegDst_Hi != AVRRegDst_Lo + 1))
        && (!(AVRRegSrc_Lo & 1)
         || !(AVRRegDst_Lo & 1)
         || (AVRRegSrc_Hi != AVRRegSrc_Lo - 1)
         || (AVRRegDst_Hi != AVRRegDst_Lo - 1)))
    {
      continue;
    }

    // swap registers if the moves come in reverse order
    if (AVRRegSrc_Lo > AVRRegSrc_Hi)
    {
      std::swap(RegSrc_Lo, RegSrc_Hi);
      std::swap(RegDst_Lo, RegDst_Hi);
    }

    BuildMI(MBB, I, dl, TII->get(AVR::MOVWRdRr), RegDst_Lo).addReg(RegSrc_Lo);
    MBB.erase(MI);
    I = MBB.erase(NextMI);
    Modified = true;
    NumMOVWS++;
  }

  return Modified;
}

bool AVR16bitInstPass::runOnMachineFunction(MachineFunction &MF)
{
  bool Modified = false;

  TII = MF.getTarget().getInstrInfo();

  for (MachineFunction::iterator MFI = MF.begin(), E = MF.end(); MFI != E;
       ++MFI)
  {
    MachineBasicBlock &MBB = *MFI;
    Modified |= InsertMovws(MBB);
  }

  return Modified;
}

FunctionPass *llvm::createAVR16bitInstPass()
{
  return new AVR16bitInstPass();
}
