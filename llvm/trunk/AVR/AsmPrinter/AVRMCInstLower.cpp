//===-- AVRMCInstLower.cpp - Convert AVR MachineInstr to an MCInst---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower AVR MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "AVRMCInstLower.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallString.h"
using namespace llvm;

MCSymbol *AVRMCInstLower::
GetGlobalAddressSymbol(const MachineOperand &MO) const 
{
  switch (MO.getTargetFlags())
  {
    default: llvm_unreachable(0 && "Unknown target flag on GV operand");
    case 0: break;
  }

  return Printer.GetGlobalValueSymbol(MO.getGlobal());
}

MCSymbol *AVRMCInstLower::
GetExternalSymbolSymbol(const MachineOperand &MO) const
{
  switch (MO.getTargetFlags())
  {
    default: assert(0 && "Unknown target flag on GV operand");
    case 0: break;
  }

  return Printer.GetExternalSymbolSymbol(MO.getSymbolName());
}

MCSymbol *AVRMCInstLower::
GetJumpTableSymbol(const MachineOperand &MO) const
{
  SmallString<256> Name;
  raw_svector_ostream(Name) << Printer.MAI->getPrivateGlobalPrefix() << "JTI"
                            << Printer.getFunctionNumber() << '_'
                            << MO.getIndex();

  switch (MO.getTargetFlags())
  {
    default: llvm_unreachable("Unknown target flag on GV operand");
    case 0: break;
  }

  // Create a symbol for the name.
  return Ctx.GetOrCreateSymbol(Name.str());
}

MCSymbol *AVRMCInstLower::
GetConstantPoolIndexSymbol(const MachineOperand &MO) const
{
  SmallString<256> Name;
  raw_svector_ostream(Name) << Printer.MAI->getPrivateGlobalPrefix() << "CPI"
                            << Printer.getFunctionNumber() << '_'
                            << MO.getIndex();

  switch (MO.getTargetFlags())
  {
    default: llvm_unreachable("Unknown target flag on GV operand");
    case 0: break;
  }

  // Create a symbol for the name.
  return Ctx.GetOrCreateSymbol(Name.str());
}

MCOperand AVRMCInstLower::
LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const
{
  // FIXME: We would like an efficient form for this, so we don't have to do a
  // lot of extra uniquing. (this comment originated from MSP430 port dev)
  const MCExpr *Expr = MCSymbolRefExpr::Create(Sym, Ctx);

  switch (MO.getTargetFlags())
  {
    default: llvm_unreachable("Unknown target flag on GV operand");
    case 0: break;
  }

  if (!MO.isJTI() && MO.getOffset())
  {
    Expr = MCBinaryExpr::CreateAdd(Expr,
                                   MCConstantExpr::Create(MO.getOffset(), Ctx),
                                   Ctx);
  }
  return MCOperand::CreateExpr(Expr);
}

void AVRMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const
{
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i)
  {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType())
    {
      default:
        MI->dump();
        assert(0 && "unknown operand type");
      case MachineOperand::MO_Register:
        // Ignore all implicit register operands.
        if (MO.isImplicit()) continue;
        MCOp = MCOperand::CreateReg(MO.getReg());
        break;
      case MachineOperand::MO_Immediate:
        MCOp = MCOperand::CreateImm(MO.getImm());
        break;
      case MachineOperand::MO_MachineBasicBlock:
        MCOp = MCOperand::CreateExpr(MCSymbolRefExpr::Create(
                           MO.getMBB()->getSymbol(Printer.OutContext), Ctx));
        break;
      case MachineOperand::MO_GlobalAddress:
        MCOp = LowerSymbolOperand(MO, GetGlobalAddressSymbol(MO));
        break;
      case MachineOperand::MO_ExternalSymbol:
        MCOp = LowerSymbolOperand(MO, GetExternalSymbolSymbol(MO));
        break;
      case MachineOperand::MO_JumpTableIndex:
        MCOp = LowerSymbolOperand(MO, GetJumpTableSymbol(MO));
        break;
      case MachineOperand::MO_ConstantPoolIndex:
        MCOp = LowerSymbolOperand(MO, GetConstantPoolIndexSymbol(MO));
        break;
    }

    OutMI.addOperand(MCOp);
  }
}
