//===-- AVRExpandPseudoInsts.cpp - Expand pseudo instructions ---*- C++ -*-===//
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

static void splitRegs(const TargetRegisterInfo *TRI, unsigned Reg,
                      unsigned &loreg, unsigned &hireg)
{
  loreg = TRI->getSubReg(Reg, AVR::sub_lo);
  hireg = TRI->getSubReg(Reg, AVR::sub_hi);
}

bool AVRExpandPseudo::expandMI(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MBBI)
{
  MachineInstr &MI = *MBBI;
  unsigned Opcode = MI.getOpcode();
  unsigned OpLo, OpHi, SrcLoReg, SrcHiReg, DstLoReg, DstHiReg;

  //:FIXME: transfer implicit operands, memory ops
  //:TODO: obviously factor out all this mess
  switch (Opcode)
  {
  case AVR::ADDWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::ADDRdRr;
      OpHi = AVR::ADCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::SUBWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::SUBRdRr;
      OpHi = AVR::SBCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::ADCWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::ADCRdRr;
      OpHi = AVR::ADCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::SBCWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::SBCRdRr;
      OpHi = AVR::SBCRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

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

      if (MI.getOperand(1).isGlobal())
      {
        const GlobalValue *GV = MI.getOperand(1).getGlobal();
        int64_t Offs = MI.getOperand(1).getOffset();
        unsigned TF = MI.getOperand(1).getTargetFlags();
        MIBLO.addGlobalAddress(GV, Offs, TF | AVRII::MO_LO);
        MIBHI.addGlobalAddress(GV, Offs, TF | AVRII::MO_HI);
      }
      else
      {
        unsigned Imm = MI.getOperand(1).getImm();
        MIBLO.addImm(Imm & 0xff);
        MIBHI.addImm((Imm >> 8) & 0xff);
      }

      MI.eraseFromParent();
      return true;
    }
  case AVR::SUBIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
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

      if (MI.getOperand(2).isGlobal())
      {
        const GlobalValue *GV = MI.getOperand(2).getGlobal();
        int64_t Offs = MI.getOperand(2).getOffset();
        unsigned TF = MI.getOperand(2).getTargetFlags();
        MIBLO.addGlobalAddress(GV, Offs, TF | AVRII::MO_NEG | AVRII::MO_LO);
        MIBHI.addGlobalAddress(GV, Offs, TF | AVRII::MO_NEG | AVRII::MO_HI);
      }
      else
      {
        unsigned Imm = MI.getOperand(2).getImm();
        MIBLO.addImm(Imm & 0xff);
        MIBHI.addImm((Imm >> 8) & 0xff);
      }

      MI.eraseFromParent();
      return true;
    }
  case AVR::SBCIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
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

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      MI.eraseFromParent();
      return true;
    }
  case AVR::ANDWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::ANDRdRr;
      OpHi = AVR::ANDRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::ORWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::ORRdRr;
      OpHi = AVR::ORRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::EORWRdRr:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(2).isKill();
      OpLo = AVR::EORRdRr;
      OpHi = AVR::EORRdRr;
      splitRegs(TRI, SrcReg, SrcLoReg, SrcHiReg);
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::ANDIWRdK:
    {
      //:FIXME: Handle %R25R24<def> = ANDIWRdK %R25R24<kill>, 255 the kill flag
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
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

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      MI.eraseFromParent();
      return true;
    }
  case AVR::ORIWRdK:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      bool SrcIsKill = MI.getOperand(1).isKill();
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

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg, getKillRegState(SrcIsKill))
          .addImm(Hi8);

      MI.eraseFromParent();
      return true;
    }
  case AVR::COMWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::COMRd;
      OpHi = AVR::COMRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg);

      MI.eraseFromParent();
      return true;
    }
  case AVR::LSLWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::LSLRd;
      OpHi = AVR::ROLRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg);

      MI.eraseFromParent();
      return true;
    }
  case AVR::LSRWRd:
    {
      unsigned DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      OpLo = AVR::RORRd;
      OpHi = AVR::LSRRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg);

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
      OpLo = AVR::RORRd;
      OpHi = AVR::ASRRd;
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstHiReg);

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(DstLoReg);

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
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      if (SrcReg != DstLoReg)
      {
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg);
      }

      if (SrcReg != DstHiReg)
      {
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
          .addReg(DstHiReg, RegState::Define)
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      }

      BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::LSLRd))
        .addReg(DstHiReg, RegState::Define)
        .addReg(DstHiReg);

      BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::SBCRdRr))
        .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
        .addReg(DstHiReg)
        .addReg(DstHiReg);

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
      splitRegs(TRI, DstReg, DstLoReg, DstHiReg);

      if (SrcReg != DstLoReg)
      {
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::MOVRdRr))
          .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(SrcReg, getKillRegState(SrcIsKill));
      }

      BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(AVR::EORRdRr))
        .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
        .addReg(DstHiReg)
        .addReg(DstHiReg);

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

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo))
          .addReg(DstReg)
          .addReg(SrcLoReg, getKillRegState(SrcIsKill));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi))
          .addReg(DstReg, getKillRegState(DstIsKill))
          .addImm(1)
          .addReg(SrcHiReg, getKillRegState(SrcIsKill));

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

      if (MI.getOperand(1).isGlobal())
      {
        const GlobalValue *GV = MI.getOperand(1).getGlobal();
        int64_t Offs = MI.getOperand(1).getOffset();
        unsigned TF = MI.getOperand(1).getTargetFlags();
        MIBLO.addGlobalAddress(GV, Offs, TF);
        MIBHI.addGlobalAddress(GV, Offs + 1, TF);
      }
      else
      {
        unsigned Imm = MI.getOperand(1).getImm();
        MIBLO.addImm(Imm);
        MIBHI.addImm(Imm + 1);
      }

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

      MachineInstrBuilder MIBLO =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpLo));

      MachineInstrBuilder MIBHI =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(OpHi));

      if (MI.getOperand(0).isGlobal())
      {
        const GlobalValue *GV = MI.getOperand(0).getGlobal();
        int64_t Offs = MI.getOperand(0).getOffset();
        unsigned TF = MI.getOperand(0).getTargetFlags();
        MIBLO.addGlobalAddress(GV, Offs, TF);
        MIBHI.addGlobalAddress(GV, Offs + 1, TF);
      }
      else
      {
        unsigned Imm = MI.getOperand(0).getImm();
        MIBLO.addImm(Imm);
        MIBHI.addImm(Imm + 1);
      }

      MIBLO.addReg(SrcLoReg, getKillRegState(SrcIsKill));
      MIBHI.addReg(SrcHiReg, getKillRegState(SrcIsKill));

      MI.eraseFromParent();
      return true;
    }
  case AVR::PUSHWRr:
    {
      //:FIXME: transfer implicit SP into splitted instructions
      //we have something like:PUSH16r %R11W<kill>, %SPW<imp-def>, %SPW<imp-use>
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
      //:FIXME: transfer implicit SP into splitted instructions
      //we have something like:%R6W<def> = POP16r %SPW<imp-def>, %SPW<imp-use>
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
