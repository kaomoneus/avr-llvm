//===-- AVRInstPrinter.cpp - Convert AVR MCInst to assembly syntax --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an AVR MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "AVR.h"
#include "AVRInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// Include the auto-generated portion of the assembly writer.
#define MachineInstr MCInst
#include "AVRGenAsmWriter.inc"
#undef MachineInstr

void AVRInstPrinter::printInst(const MCInst *MI, raw_ostream &O)
{
  printInstruction(MI, O);//TableGen'd: AVRInstrInfo.td --> AVRGenAsmWriter.inc
}

void AVRInstPrinter::printPCRelImmOperand(const MCInst *MI, unsigned OpNo,
                                             raw_ostream &O)
{
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << Op.getImm();
  else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    O << Op.getExpr();
  }
}

void AVRInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O, const char *Modifier) 
{
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg())
  {
    O << getRegisterName(Op.getReg());
  }
  else if (Op.isImm())
  {
    O << Op.getImm();
  }
  else
  {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << *Op.getExpr();
  }
}

void AVRInstPrinter::printSrcMemOperand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O, const char *Modifier)
{
  const MCOperand &Base = MI->getOperand(OpNo);
  const MCOperand &Disp = MI->getOperand(OpNo+1);

  // Print displacement first
  if (Disp.isExpr()) {
    O << '&';
    O << Disp.getExpr();
  } else {
    assert(Disp.isImm() && "Expected immediate in displacement field");
    if (!Base.getReg())
      O << '&';

    O << Disp.getImm();
  }


  // Print register base field
  if (Base.getReg()) {
    O << '(' << getRegisterName(Base.getReg()) << ')';
  }
}
