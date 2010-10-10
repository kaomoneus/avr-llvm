//===--------- AVRAsmPrinter.cpp - AVR LLVM assembly writer ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format AVR assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "AVR.h"
#include "AVRInstrInfo.h"
#include "AVRTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

//:TODO: This file has been copied from another target without customizing
// to get asm code fast, but it must be completely rewritten
// Change coding standards, tweak AVR asm directives, etc...

namespace {
  struct AVRAsmPrinter : public AsmPrinter {
    /// Unique incrementer for label values for referencing Global values.
    ///

    explicit AVRAsmPrinter(TargetMachine &tm, MCStreamer &Streamer)
      : AsmPrinter(tm, Streamer) {}

    virtual const char *getPassName() const {
      return "AVR Assembly Printer";
    }
    void printInstruction(const MachineInstr *MI, raw_ostream &O);
    void EmitInstruction(const MachineInstr *MI) {
      SmallString<128> Str;
      raw_svector_ostream OS(Str);
      printInstruction(MI, OS);
      OutStreamer.EmitRawText(OS.str());
    }
    static const char *getRegisterName(unsigned RegNo);

    void printOp(const MachineOperand &MO, raw_ostream &O);
    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
    virtual void EmitFunctionBodyStart();
    virtual void EmitFunctionBodyEnd();
    void EmitStartOfAsmFile(Module &M);

    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                         unsigned AsmVariant, const char *ExtraCode,
                         raw_ostream &O);
    bool PrintAsmMemoryOperand(const MachineInstr *MI,
                               unsigned OpNo, unsigned AsmVariant,
                               const char *ExtraCode, raw_ostream &O);
  };
} // end of anonymous namespace

#include "AVRGenAsmWriter.inc"

void AVRAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                   raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  if (MO.isReg()) {
    assert(TargetRegisterInfo::isPhysicalRegister(MO.getReg()) &&
           "Not physreg??");
    O << getRegisterName(MO.getReg());
  } else if (MO.isImm()) {
    O << MO.getImm();
    assert(MO.getImm() < (1 << 30));
  } else {
    printOp(MO, O);
  }
}


void AVRAsmPrinter::printOp(const MachineOperand &MO, raw_ostream &O) {
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << getRegisterName(MO.getReg());
    return;

  case MachineOperand::MO_Immediate:
    llvm_unreachable("printOp() does not handle immediate values");
    return;

  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol();
    return;

  case MachineOperand::MO_ConstantPoolIndex:
    O << MAI->getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << "_"
      << MO.getIndex();
    return;

  case MachineOperand::MO_ExternalSymbol:
    O << MO.getSymbolName();
    return;

  case MachineOperand::MO_GlobalAddress:
    O << *Mang->getSymbol(MO.getGlobal());
    return;

  case MachineOperand::MO_JumpTableIndex:
    O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber()
      << '_' << MO.getIndex();
    return;

  default:
    O << "<unknown operand type: " << MO.getType() << ">";
    return;
  }
}

/// EmitFunctionBodyStart - Targets can override this to emit stuff before
/// the first basic block in the function.
void AVRAsmPrinter::EmitFunctionBodyStart() {
  OutStreamer.EmitRawText("\t.ent " + Twine(CurrentFnSym->getName()));
}

/// EmitFunctionBodyEnd - Targets can override this to emit stuff after
/// the last basic block in the function.
void AVRAsmPrinter::EmitFunctionBodyEnd() {
  OutStreamer.EmitRawText("\t.end " + Twine(CurrentFnSym->getName()));
}

void AVRAsmPrinter::EmitStartOfAsmFile(Module &M) {
  OutStreamer.EmitRawText(StringRef("\t.set noat"));
}

/// PrintAsmOperand - Print out an operand for an inline asm expression.
///
bool AVRAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                      unsigned AsmVariant,
                                      const char *ExtraCode, raw_ostream &O) {
  printOperand(MI, OpNo, O);
  return false;
}

bool AVRAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                            unsigned OpNo, unsigned AsmVariant,
                                            const char *ExtraCode,
                                            raw_ostream &O) {
  if (ExtraCode && ExtraCode[0])
    return true; // Unknown modifier.
  O << "0(";
  printOperand(MI, OpNo, O);
  O << ")";
  return false;
}

// Force static initialization.
extern "C" void LLVMInitializeAVRAsmPrinter() {
  RegisterAsmPrinter<AVRAsmPrinter> X(TheAVRTarget);
}
