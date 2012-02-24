//===-- AVRFrameLowering.cpp - AVR Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the AVR implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "AVR.h"
#include "AVRFrameLowering.h"
#include "AVRInstrInfo.h"
#include "AVRMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

AVRFrameLowering::AVRFrameLowering() :
  TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 1, -2) {}

bool AVRFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const
{
  //:TODO: for now never reserve call frame space in the prologue
  return false;
}

void AVRFrameLowering::emitPrologue(MachineFunction &MF) const
{
  // Early exit if the frame pointer is not needed in this function.
  if (!hasFP(MF))
  {
    return;
  }

  MachineBasicBlock &MBB = MF.front();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  DebugLoc dl = (MBBI != MBB.end()) ? MBBI->getDebugLoc() : DebugLoc();
  const AVRMachineFunctionInfo *AFI = MF.getInfo<AVRMachineFunctionInfo>();
  uint64_t FrameSize = MFI->getStackSize() - AFI->getCalleeSavedFrameSize();
  const AVRInstrInfo &TII =
    *static_cast<const AVRInstrInfo *>(MF.getTarget().getInstrInfo());

  // Skip the callee-saved push instructions.
  while ((MBBI != MBB.end())
         && ((MBBI->getOpcode() == AVR::PUSHRr
             || MBBI->getOpcode() == AVR::PUSHWRr)))
  {
    ++MBBI;
  }

  // Update Y with the new base value.
  BuildMI(MBB, MBBI, dl, TII.get(AVR::SPLOAD), AVR::R29R28).addReg(AVR::SP)
    .setMIFlag(MachineInstr::FrameSetup);

  // Mark the FramePtr as live-in in every block except the entry.
  for (MachineFunction::iterator I = llvm::next(MF.begin()), E = MF.end();
       I != E; ++I)
  {
    I->addLiveIn(AVR::R29R28);
  }

  // Reserve the necessary frame memory by doing FP -= <size>.
  if (FrameSize)
  {
    unsigned Opcode = (FrameSize > 63) ? AVR::SUBIWRdK : AVR::SBIWRdK;

    MachineInstr *MI = BuildMI(MBB, MBBI, dl, TII.get(Opcode), AVR::R29R28)
      .addReg(AVR::R29R28).addImm(FrameSize)
      .setMIFlag(MachineInstr::FrameSetup);
    // The SRW implicit def is dead.
    //MI->getOperand(3).setIsDead(); :TODO:
    (void)MI;
  }
}

void AVRFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const
{
  // Early exit if the frame pointer is not needed in this function.
  if (!hasFP(MF))
  {
    return;
  }

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  assert(MBBI->getDesc().isReturn()
         && "Can only insert epilog into returning blocks");
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  const AVRMachineFunctionInfo *AFI = MF.getInfo<AVRMachineFunctionInfo>();
  uint64_t FrameSize = MFI->getStackSize() - AFI->getCalleeSavedFrameSize();
  const AVRInstrInfo &TII =
    *static_cast<const AVRInstrInfo *>(MF.getTarget().getInstrInfo());

  // Early exit if there is no need to restore the frame pointer.
  if (!FrameSize)
  {
    return;
  }

  // Skip the callee-saved pop instructions.
  while (MBBI != MBB.begin())
  {
    MachineBasicBlock::iterator PI = llvm::prior(MBBI);
    unsigned Opc = PI->getOpcode();

    if (((Opc != AVR::POPRd) && (Opc != AVR::POPWRd)) && !(PI->isTerminator()))
    {
      break;
    }

    --MBBI;
  }

  unsigned Opcode;
  DebugLoc dl = MBBI->getDebugLoc();

  // Select the optimal opcode depending on how big it is.
  if (FrameSize < 64)
  {
    Opcode = AVR::ADIWRdK;
  }
  else
  {
    Opcode = AVR::SUBIWRdK;
    FrameSize = -FrameSize;
  }

  // Update Y with the new base value.
  BuildMI(MBB, MBBI, dl, TII.get(AVR::SPLOAD), AVR::R29R28).addReg(AVR::SP);

  // Restore the frame pointer by doing FP += <size>.
  MachineInstr *MI = BuildMI(MBB, MBBI, dl, TII.get(Opcode), AVR::R29R28)
    .addReg(AVR::R29R28).addImm(FrameSize);
  // The SRW implicit def is dead.
  //MI->getOperand(3).setIsDead(); :TODO:
  (void)MI;
}

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register. This is true if the function meets any of the following
// conditions:
//  - has variable sized allocas
//  - frame pointer elimination is disabled
//  - a register has been spilled
//  - has allocas
//  - input arguments are passed using the stack
bool AVRFrameLowering::hasFP(const MachineFunction &MF) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  const AVRMachineFunctionInfo *FuncInfo = MF.getInfo<AVRMachineFunctionInfo>();
  assert(!MFI->hasVarSizedObjects() && "Variable stk objs are not allowed now");

  return (MF.getTarget().Options.DisableFramePointerElim(MF)
          || MFI->hasVarSizedObjects()
          || FuncInfo->getHasSpills()
          || FuncInfo->getHasAllocas()
          || FuncInfo->getHasStackArgs());
}

bool AVRFrameLowering::
spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MI,
                          const std::vector<CalleeSavedInfo> &CSI,
                          const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
  {
    return false;
  }

  unsigned CalleeFrameSize = 0;
  DebugLoc DL = MBB.findDebugLoc(MI);
  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();
  AVRMachineFunctionInfo *AVRFI = MF.getInfo<AVRMachineFunctionInfo>();

  for (unsigned i = CSI.size(); i != 0; --i)
  {
    unsigned Reg = CSI[i - 1].getReg();

    // Add the callee-saved register as live-in. It's killed at the spill.
    MBB.addLiveIn(Reg);
    //:FIXME: make this work with 8bit regs
    assert(TRI->getMinimalPhysRegClass(Reg)->getSize() == 2
           && "Pushing to the stack an 8 bit regiter");
    BuildMI(MBB, MI, DL, TII.get(AVR::PUSHWRr)).addReg(Reg, RegState::Kill)
      .setMIFlag(MachineInstr::FrameSetup);
    CalleeFrameSize += 2; //:TODO: when 1byte pushes work, adjust this code
  }

  AVRFI->setCalleeSavedFrameSize(CalleeFrameSize);

  return true;
}

bool AVRFrameLowering::
restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI,
                            const std::vector<CalleeSavedInfo> &CSI,
                            const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
  {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  const MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; ++i)
  {
    unsigned Reg = CSI[i].getReg();
    //:FIXME: make this work with 8bit regs
    assert(TRI->getMinimalPhysRegClass(Reg)->getSize() == 2
           && "Popping from the stack an 8 bit regiter");
    BuildMI(MBB, MI, DL, TII.get(AVR::POPWRd), Reg);
  }

  return true;
}

void AVRFrameLowering::
processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                     RegScavenger *RS) const
{
  // Spill register Y when it is used as the frame pointer.
  if (hasFP(MF))
  {
    MF.getRegInfo().setPhysRegUsed(AVR::R29R28);
  }
}

namespace
{
  struct AVRFrameAnalyzer : public MachineFunctionPass
  {
    static char ID;
    AVRFrameAnalyzer() : MachineFunctionPass(ID) {}

    bool runOnMachineFunction(MachineFunction &MF)
    {
      const MachineFrameInfo *MFI = MF.getFrameInfo();
      AVRMachineFunctionInfo *FuncInfo = MF.getInfo<AVRMachineFunctionInfo>();

      // If there are non fixed frame indices during this stage it means there
      // are allocas present in the function.
      if (MFI->getNumObjects() - MFI->getNumFixedObjects())
      {
        FuncInfo->setHasAllocas(true);
      }

      // If there are fixed frame indices present, scan the function to see if
      // they are really being used.
      if (MFI->getNumFixedObjects() == 0)
      {
        return false;
      }

      for (MachineFunction::const_iterator BB = MF.begin(), BBE = MF.end();
           BB != BBE; ++BB)
      {
        for (MachineBasicBlock::const_iterator I = (*BB).begin(),
             E = (*BB).end(); I != E; ++I)
        {
          const MachineInstr *MI = I;
          int Opcode = MI->getOpcode();

          if ((Opcode == AVR::LDDRdPtrQ) || (Opcode == AVR::LDDWRdPtrQ)
              || (Opcode == AVR::STDPtrQRr) || (Opcode == AVR::STDWPtrQRr))
          {
            for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i)
            {
              const MachineOperand &MO = MI->getOperand(i);

              if (!MO.isFI())
              {
                continue;
              }

              int Index = MO.getIndex();
              if (MFI->isFixedObjectIndex(Index))
              {
                FuncInfo->setHasStackArgs(true);
                return false;
              }
            }
          }
        }
      }

      return false;
    }

    const char *getPassName() const
    {
      return "AVR Frame Analyzer";
    }
  };

  char AVRFrameAnalyzer::ID = 0;
} // end of anonymous namespace

/// createAVRFrameAnalyzerPass - returns an instance of the frame analyzer pass.
FunctionPass *llvm::createAVRFrameAnalyzerPass()
{
  return new AVRFrameAnalyzer();
}
