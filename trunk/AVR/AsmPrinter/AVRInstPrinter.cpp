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
#include "AVRInstrInfo.h"
#include "AVRInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;


// Include the auto-generated portion of the assembly writer.
#define MachineInstr MCInst
#define NO_ASM_WRITER_BOILERPLATE
#include "AVRGenAsmWriter.inc"
#undef MachineInstr

void AVRInstPrinter::printInst(const MCInst *MI) {
  printInstruction(MI);
}

void AVRInstPrinter::printPCRelImmOperand(const MCInst *MI, unsigned OpNo) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << Op.getImm();
  else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    Op.getExpr()->print(O, &MAI);
  }
}

void AVRInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << getRegisterName(Op.getReg());
  } else if (Op.isImm()) {
    O << '#' << Op.getImm();
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << '#';
    Op.getExpr()->print(O, &MAI);
  }
}

void AVRInstPrinter::printSrcMemOperand(const MCInst *MI, unsigned OpNo,
                                           const char *Modifier) {
  const MCOperand &Base = MI->getOperand(OpNo);
  const MCOperand &Disp = MI->getOperand(OpNo+1);

  // Print displacement first
  if (Disp.isExpr()) {
    O << '&';
    Disp.getExpr()->print(O, &MAI);
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

void AVRInstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo) {
  unsigned CC = MI->getOperand(OpNo).getImm();
/* MSP430CC defined in MSP430.h
  switch (CC) {
  default:
   llvm_unreachable("Unsupported CC code");
   break;
  case AVRCC::COND_E:
   O << "eq";
   break;
  case AVRCC::COND_NE:
   O << "ne";
   break;
  case AVRCC::COND_HS:
   O << "hs";
   break;
  case AVRCC::COND_LO:
   O << "lo";
   break;
  case AVRCC::COND_GE:
   O << "ge";
   break;
  case AVRCC::COND_L:
   O << 'l';
   break;
  }*/
}
