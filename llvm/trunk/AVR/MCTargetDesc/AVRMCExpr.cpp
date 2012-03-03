//===-- AVRMCExpr.cpp - AVR specific MC expression classes ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AVRMCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCAssembler.h"

using namespace llvm;

const AVRMCExpr *
AVRMCExpr::Create(VariantKind Kind, const MCExpr *Expr, MCContext &Ctx)
{
  return new (Ctx) AVRMCExpr(Kind, Expr);
}

void AVRMCExpr::PrintImpl(raw_ostream &OS) const
{
  switch (Kind)
  {
  default:                      llvm_unreachable("Invalid kind!");
  case VK_AVR_HI8:              OS << "hi8("; break;
  case VK_AVR_LO8:              OS << "lo8("; break;
  }

  const MCExpr *Expr = getSubExpr();
  if (Expr->getKind() != MCExpr::SymbolRef) OS << '(';
  Expr->print(OS);
  if (Expr->getKind() != MCExpr::SymbolRef) OS << ')';

  OS << ')';
}

bool
AVRMCExpr::EvaluateAsRelocatableImpl(MCValue &Res,
                                     const MCAsmLayout *Layout) const
{
  return false;
}

static void AddValueSymbols_(const MCExpr *Value, MCAssembler *Asm)
{
  switch (Value->getKind())
  {
  case MCExpr::Target:
    llvm_unreachable("Can't handle nested target expr!");
  case MCExpr::Constant:
    break;
  case MCExpr::Binary:
    {
      const MCBinaryExpr *BE = cast<MCBinaryExpr>(Value);
      AddValueSymbols_(BE->getLHS(), Asm);
      AddValueSymbols_(BE->getRHS(), Asm);
      break;
    }
  case MCExpr::SymbolRef:
    Asm->getOrCreateSymbolData(cast<MCSymbolRefExpr>(Value)->getSymbol());
    break;
  case MCExpr::Unary:
    AddValueSymbols_(cast<MCUnaryExpr>(Value)->getSubExpr(), Asm);
    break;
  }
}

void AVRMCExpr::AddValueSymbols(MCAssembler *Asm) const
{
  AddValueSymbols_(getSubExpr(), Asm);
}
