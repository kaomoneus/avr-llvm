//===-- AVRExpandPseudoInsts.cpp - Expand pseudo instructions -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a pass that expands pseudo instructions into target
// instructions. This pass should be run after register allocation but before
// the post-regalloc scheduling pass.
//
//===----------------------------------------------------------------------===//

#include "AVR.h"
#include "AVRInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetRegisterInfo.h"

using namespace llvm;

namespace
{

class AVRExpandPseudo : public MachineFunctionPass
{
public:
  static char ID;
  const TargetRegisterInfo *TRI;
  const TargetInstrInfo *TII;
  AVRExpandPseudo() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF);

  const char *getPassName() const
  {
    return "AVR pseudo instruction expansion pass";
  }
private:
  bool expandMBB(MachineBasicBlock &MBB);
  bool expandMI(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI);
};

char AVRExpandPseudo::ID = 0;

} // end of anonymous namespace

/// splitRegs - Split the incoming register pair into the subregisters that it
/// is composed of.
static void splitRegs(const TargetRegisterInfo *TRI, unsigned Reg,
                      unsigned &LoReg, unsigned &HiReg)
{
  LoReg = TRI->getSubReg(Reg, AVR::sub_lo);
  HiReg = TRI->getSubReg(Reg, AVR::sub_hi);
}

bool AVRExpandPseudo::expandMI(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MBBI)
{
  MachineInstr &MI = *MBBI;
  int Opcode = MI.getOpcode();
  unsigned OpLo, OpHi, SrcLoReg, SrcHiReg, DstLoReg, DstHiReg;

  //:TODO: obviously factor out all this mess
  switch (Opcode)
  {
  case AVR::ADDWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::ADDRdRr;
      OpHi = AVR::ADCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ADCWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::ADCRdRr;
      OpHi = AVR::ADCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly killed
      MIBLO->getOperand(4).setIsKill();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::SUBWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::SUBRdRr;
      OpHi = AVR::SBCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::SUBIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::SUBIRdK;
      OpHi = AVR::SBCIRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill));

      switch (MI.getOperand(2).getType())
      {
      case MachineOperand::MO_GlobalAddress:
        {
          const GlobalValue *GV = MI.getOperand(2).getGlobal();
          int64_t Offs = MI.getOperand(2).getOffset();
          unsigned TF = MI.getOperand(2).getTargetFlags();
          MIBLO.addGlobalAddress(GV, Offs, TF | AVRII::MO_NEG | AVRII::MO_LO);
          MIBHI.addGlobalAddress(GV, Offs, TF | AVRII::MO_NEG | AVRII::MO_HI);
          break;
        }
      case MachineOperand::MO_Immediate:
        {
          unsigned Imm = MI.getOperand(2).getImm();
          MIBLO.addImm(Imm & 0xff);
          MIBHI.addImm((Imm >> 8) & 0xff);
          break;
        }
      default:
        llvm_unreachable("Unknown operand type!");
      }

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::SBCWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::SBCRdRr;
      OpHi = AVR::SBCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly killed
      MIBLO->getOperand(4).setIsKill();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::SBCIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      unsigned Imm = MI.getOperand(2).getImm();
      unsigned Lo8 = Imm & 0xff;
      unsigned Hi8 = (Imm >> 8) & 0xff;
      OpLo = AVR::SBCIRdK;
      OpHi = AVR::SBCIRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(SrcIsKill))
          .addImm(Lo8);
      // SREG is always implicitly killed
      MIBLO->getOperand(4).setIsKill();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ANDWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::ANDRdRr;
      OpHi = AVR::ANDRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly dead
      MIBLO->getOperand(3).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ANDIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      unsigned Imm = MI.getOperand(2).getImm();
      unsigned Lo8 = Imm & 0xff;
      unsigned Hi8 = (Imm >> 8) & 0xff;
      OpLo = AVR::ANDIRdK;
      OpHi = AVR::ANDIRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(SrcIsKill))
          .addImm(Lo8);
      // SREG is always implicitly dead
      MIBLO->getOperand(3).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ORWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::ORRdRr;
      OpHi = AVR::ORRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly dead
      MIBLO->getOperand(3).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ORIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      unsigned Imm = MI.getOperand(2).getImm();
      unsigned Lo8 = Imm & 0xff;
      unsigned Hi8 = (Imm >> 8) & 0xff;
      OpLo = AVR::ORIRdK;
      OpHi = AVR::ORIRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(SrcIsKill))
          .addImm(Lo8);
      // SREG is always implicitly dead
      MIBLO->getOperand(3).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::EORWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      bool ImpIsDead = MI.getOperand(3).isDead();
      OpLo = AVR::EORRdRr;
      OpHi = AVR::EORRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly dead
      MIBLO->getOperand(3).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::COMWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::COMRd;
      OpHi = AVR::COMRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill));
      // SREG is always implicitly dead
      MIBLO->getOperand(2).setIsDead();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill));

      if (ImpIsDead) MIBHI->getOperand(2).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  case AVR::CPWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsKill = MI.getOperand(0).isKill();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::CPRdRr;
      OpHi = AVR::CPCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(2).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(3).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::CPCWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsKill = MI.getOperand(0).isKill();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::CPCRdRr;
      OpHi = AVR::CPCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, getKillRegState(DstIsKill))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));
      // SREG is always implicitly killed
      MIBLO->getOperand(3).setIsKill();

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, getKillRegState(DstIsKill))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      if (ImpIsDead) MIBHI->getOperand(2).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(3).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::LDIRdK;
      OpHi = AVR::LDIRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead));

      switch (MI.getOperand(1).getType())
      {
      case MachineOperand::MO_GlobalAddress:
        {
          const GlobalValue *GV = MI.getOperand(1).getGlobal();
          int64_t Offs = MI.getOperand(1).getOffset();
          unsigned TF = MI.getOperand(1).getTargetFlags();
          MIBLO.addGlobalAddress(GV, Offs, TF | AVRII::MO_LO);
          MIBHI.addGlobalAddress(GV, Offs, TF | AVRII::MO_HI);
          break;
        }
      case MachineOperand::MO_BlockAddress:
        {
          const BlockAddress *BA = MI.getOperand(1).getBlockAddress();
          unsigned TF = MI.getOperand(1).getTargetFlags();
          MIBLO.addOperand(MachineOperand::CreateBA(BA, TF | AVRII::MO_LO));
          MIBHI.addOperand(MachineOperand::CreateBA(BA, TF | AVRII::MO_HI));
          break;
        }
      case MachineOperand::MO_Immediate:
        {
          unsigned Imm = MI.getOperand(1).getImm();
          MIBLO.addImm(Imm & 0xff);
          MIBHI.addImm((Imm >> 8) & 0xff);
          break;
        }
      default:
        llvm_unreachable("Unknown operand type!");
      }

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDSWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::LDSRdK;
      OpHi = AVR::LDSRdK;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead));

      switch (MI.getOperand(1).getType())
      {
      case MachineOperand::MO_GlobalAddress:
        {
          const GlobalValue *GV = MI.getOperand(1).getGlobal();
          int64_t Offs = MI.getOperand(1).getOffset();
          unsigned TF = MI.getOperand(1).getTargetFlags();
          MIBLO.addGlobalAddress(GV, Offs, TF);
          MIBHI.addGlobalAddress(GV, Offs + 1, TF);
          break;
        }
      case MachineOperand::MO_Immediate:
        {
          unsigned Imm = MI.getOperand(1).getImm();
          MIBLO.addImm(Imm);
          MIBHI.addImm(Imm + 1);
          break;
        }
      default:
        llvm_unreachable("Unknown operand type!");
      }

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDWRdPtr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      OpLo = AVR::LDRdPtr;
      OpHi = AVR::LDDRdPtrQ;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, getKillRegState(SrcIsKill))
          .addImm(1);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDWRdPtrPi:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsDead = MI.getOperand(1).isKill();
      OpLo = AVR::LDRdPtrPi;
      OpHi = AVR::LDRdPtrPi;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, RegState::Define)
          .addReg(SrcReg, RegState::Kill);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, RegState::Define | getDeadRegState(SrcIsDead))
          .addReg(SrcReg, RegState::Kill);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDWRdPtrPd:
    {
      //:TODO: verify this expansion
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsDead = MI.getOperand(1).isKill();
      OpLo = AVR::LDRdPtrPd;
      OpHi = AVR::LDRdPtrPd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, RegState::Define)
          .addReg(SrcReg, RegState::Kill);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, RegState::Define | getDeadRegState(SrcIsDead))
          .addReg(SrcReg, RegState::Kill);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::LDDWRdPtrQ:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      unsigned Imm = MI.getOperand(2).getImm();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      OpLo = AVR::LDDRdPtrQ;
      OpHi = AVR::LDDRdPtrQ;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg)
          .addImm(Imm);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, getKillRegState(SrcIsKill))
          .addImm(Imm + 1);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::STSWKRr:
    {
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool SrcIsKill = MI.getOperand(1).isKill();
      OpLo = AVR::STSKRr;
      OpHi = AVR::STSKRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      // Write the high byte first in case this address belongs to a special
      // I/O address with a special temporary register.
      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi));

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo));

      switch (MI.getOperand(0).getType())
      {
      case MachineOperand::MO_GlobalAddress:
        {
          const GlobalValue *GV = MI.getOperand(0).getGlobal();
          int64_t Offs = MI.getOperand(0).getOffset();
          unsigned TF = MI.getOperand(0).getTargetFlags();
          MIBLO.addGlobalAddress(GV, Offs, TF);
          MIBHI.addGlobalAddress(GV, Offs + 1, TF);
          break;
        }
      case MachineOperand::MO_Immediate:
        {
          unsigned Imm = MI.getOperand(0).getImm();
          MIBLO.addImm(Imm);
          MIBHI.addImm(Imm + 1);
          break;
        }
      default:
        llvm_unreachable("Unknown operand type!");
      }

      MIBLO.addReg(SrcLoReg, getKillRegState(SrcIsKill));
      MIBHI.addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::STWPtrRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsKill = MI.getOperand(0).isKill();
      bool SrcIsKill = MI.getOperand(1).isKill();
      OpLo = AVR::STPtrRr;
      OpHi = AVR::STDPtrQRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      //:TODO: need to reverse this order like inw and stsw?
      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstReg, getKillRegState(DstIsKill))
          .addImm(1)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::STWPtrPiRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      unsigned Imm = MI.getOperand(3).getImm();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::STPtrPiRr;
      OpHi = AVR::STPtrPiRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstReg, RegState::Define)
          .addReg(DstReg, RegState::Kill)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill))
          .addImm(Imm);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstReg, RegState::Kill)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill))
          .addImm(Imm);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::STWPtrPdRr:
    {
      //:TODO: verify this expansion
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      unsigned Imm = MI.getOperand(3).getImm();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::STPtrPdRr;
      OpHi = AVR::STPtrPdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstReg, RegState::Define)
          .addReg(DstReg, RegState::Kill)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill))
          .addImm(Imm);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstReg, RegState::Kill)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill))
          .addImm(Imm);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::STDWPtrQRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      unsigned Imm = MI.getOperand(1).getImm();
      bool DstIsKill = MI.getOperand(0).isKill();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::STDPtrQRr;
      OpHi = AVR::STDPtrQRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstReg)
          .addImm(Imm)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstReg, getKillRegState(DstIsKill))
          .addImm(Imm + 1)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::INWRdA:
    {
      unsigned Imm = MI.getOperand(1).getImm();
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::INRdA;
      OpHi = AVR::INRdA;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addImm(Imm);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addImm(Imm + 1);

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::OUTWARr:
    {
      unsigned Imm = MI.getOperand(0).getImm();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool SrcIsKill = MI.getOperand(1).isKill();
      OpLo = AVR::OUTARr;
      OpHi = AVR::OUTARr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      // 16 bit I/O writes need the high byte first
      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addImm(Imm + 1)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addImm(Imm)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MIBLO->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());
      MIBHI->setMemRefs(MI.memoperands_begin(), MI.memoperands_end());

      MI.eraseFromParent();
      return true;
    }
  case AVR::PUSHWRr:
    {
      unsigned SrcReg = MI.getOperand(0).getReg();
      bool SrcIsKill = MI.getOperand(0).isKill();
      OpLo = AVR::PUSHRr;
      OpHi = AVR::PUSHRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::POPWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      OpLo = AVR::POPRd;
      OpHi = AVR::POPRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi), DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo), DstLoReg);

      MI.eraseFromParent();
      return true;
    }
  case AVR::LSLWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::LSLRd;
      OpHi = AVR::ROLRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill));

      if (ImpIsDead) MIBHI->getOperand(2).setIsDead();
      // SREG is always implicitly killed
      MIBHI->getOperand(3).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::LSRWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::RORRd;
      OpHi = AVR::LSRRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill));

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill));

      if (ImpIsDead) MIBLO->getOperand(2).setIsDead();
      // SREG is always implicitly killed
      MIBLO->getOperand(3).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::RORWRd:
  case AVR::ROLWRd:
    assert(0 && "ROLW and RORW unimplemented");
  case AVR::ASRWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool DstIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      OpLo = AVR::RORRd;
      OpHi = AVR::ASRRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(DstIsKill));

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg, getKillRegState(DstIsKill));

      if (ImpIsDead) MIBLO->getOperand(2).setIsDead();
      // SREG is always implicitly killed
      MIBLO->getOperand(3).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::SEXT:
    {
      /*
        sext R17:R16, R17
        mov     r16, r17
        lsl     r17
        sbc     r17, r17
        sext R17:R16, R13
        mov     r16, r13
        mov     r17, r13
        lsl     r17
        sbc     r17, r17
        sext R17:R16, R16
        mov     r17, r16
        lsl     r17
        sbc     r17, r17
      */
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      if (SrcReg != DstLoReg)
      {
        MachineInstrBuilder MOV =
          BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
            .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
            .addReg(SrcReg);
        if (SrcReg == DstHiReg) MOV->getOperand(1).setIsKill();
      }

      if (SrcReg != DstHiReg)
      {
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
          .addReg(DstHiReg, RegState::Define)
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      }

      BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::LSLRd))
        .addReg(DstHiReg, RegState::Define)
        .addReg(DstHiReg, RegState::Kill);

      MachineInstrBuilder SBC =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::SBCRdRr))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, RegState::Kill)
          .addReg(DstHiReg, RegState::Kill);

      if (ImpIsDead) SBC->getOperand(3).setIsDead();
      // SREG is always implicitly killed
      SBC->getOperand(4).setIsKill();

      MI.eraseFromParent();
      return true;
    }
  case AVR::ZEXT:
    {
      /*
       zext R25:R24, R20
       mov      R24, R20
       eor      R25, R25
       zext R25:R24, R24
       eor      R25, R25
       zext R25:R24, R25
       mov      R24, R25
       eor      R25, R25
       */
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(1).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
      bool ImpIsDead = MI.getOperand(2).isDead();
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      if (SrcReg != DstLoReg)
      {
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      }

      MachineInstrBuilder EOR =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::EORRdRr))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, RegState::Kill)
          .addReg(DstHiReg, RegState::Kill);

      if (ImpIsDead) EOR->getOperand(3).setIsDead();

      MI.eraseFromParent();
      return true;
    }
  }

  return false;
}

bool AVRExpandPseudo::expandMBB(MachineBasicBlock &MBB)
{
  bool Modified = false;

  MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
  while (MBBI != E)
  {
    MachineBasicBlock::iterator NMBBI = llvm::next(MBBI);
    Modified |= expandMI(MBB, MBBI);
    MBBI = NMBBI;
  }

  return Modified;
}

bool AVRExpandPseudo::runOnMachineFunction(MachineFunction &MF)
{
  bool Modified = false;
  const TargetMachine &TM = MF.getTarget();
  TRI = TM.getRegisterInfo();
  TII = MF.getTarget().getInstrInfo();

  for (MachineFunction::iterator MFI = MF.begin(), E = MF.end(); MFI != E;
       ++MFI)
  {
    Modified |= expandMBB(*MFI);
  }

  return Modified;
}

/// createAVRExpandPseudoPass - returns an instance of the pseudo instruction
/// expansion pass.
FunctionPass *llvm::createAVRExpandPseudoPass()
{
  return new AVRExpandPseudo();
}
