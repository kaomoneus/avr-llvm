//===-- AVRAsmPrinter.cpp - AVR LLVM assembly writer ----------------------===//
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

#define DEBUG_TYPE "avr-asm-printer"

#include "AVR.h"
#include "AVRMCInstLower.h"
#include "InstPrinter/AVRInstPrinter.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/Mangler.h"

using namespace llvm;

namespace
{

class AVRAsmPrinter : public AsmPrinter
{
public:
  explicit AVRAsmPrinter(TargetMachine &TM, MCStreamer &Streamer) :
    AsmPrinter(TM, Streamer) {}

  const char *getPassName() const
  {
    return "AVR Assembly Printer";
  }

  void printOperand(const MachineInstr *MI, unsigned OpNo, raw_ostream &O,
                    const char *Modifier = 0);

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                       unsigned AsmVariant, const char *ExtraCode,
                       raw_ostream &O) LLVM_OVERRIDE;

  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                                     unsigned AsmVariant, const char *ExtraCode,
                                     raw_ostream &O) LLVM_OVERRIDE;

public: // AsmPrinter
  void EmitInstruction(const MachineInstr *MI);
};

} // end of anonymous namespace

void AVRAsmPrinter::printOperand(const MachineInstr *MI, unsigned OpNo,
                                 raw_ostream &O, const char *Modifier)
{
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType())
  {
  case MachineOperand::MO_Register:
    O << AVRInstPrinter::getRegisterName(MO.getReg());
    break;
  case MachineOperand::MO_Immediate:
    O << MO.getImm();
    break;
  case MachineOperand::MO_GlobalAddress:
    O << *Mang->getSymbol(MO.getGlobal());
    break;
  case MachineOperand::MO_ExternalSymbol:
    O << *GetExternalSymbolSymbol(MO.getSymbolName());
    break;
  //:FIXME: readd this once needed
    /*
  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol();
    break;
    */
  default:
    llvm_unreachable("Not implemented yet!");
  }
}

bool AVRAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                                    unsigned AsmVariant, const char *ExtraCode,
                                    raw_ostream &O)
{
  // Default asm printer can only deal with some extra codes,
  // so try it first.
  bool Error = AsmPrinter::PrintAsmOperand(MI, OpNum, AsmVariant, ExtraCode, O);
  if (Error && ExtraCode && ExtraCode[0])
  {
    if (ExtraCode[1] != 0) return true; // Unknown modifier.
  }

  printOperand(MI, OpNum, O);

  return false;
}

bool AVRAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                          unsigned OpNum, unsigned AsmVariant,
                                          const char *ExtraCode,
                                          raw_ostream &O) {
  // FIXME: Copy-pasted ARM yet.
  // Does this asm operand have a single letter operand modifier?
  if (ExtraCode && ExtraCode[0]) {
    if (ExtraCode[1] != 0) return true; // Unknown modifier.

    switch (ExtraCode[0]) {
      default: return true;  // Unknown modifier.
      case 'm': // The base register of a memory operand.
        if (!MI->getOperand(OpNum).isReg())
          return true;
        O << AVRInstPrinter::getRegisterName(MI->getOperand(OpNum).getReg());
        return false;
    }
  }

  const MachineOperand &MO = MI->getOperand(OpNum);
  assert(MO.isReg() && "unexpected inline asm memory operand");
  O << "[" << AVRInstPrinter::getRegisterName(MO.getReg()) << "]";
  return false;
}

//===----------------------------------------------------------------------===//
void AVRAsmPrinter::EmitInstruction(const MachineInstr *MI)
{
  AVRMCInstLower MCInstLowering(OutContext, *this);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst);
}

//===----------------------------------------------------------------------===//
// Target Registry Stuff
//===----------------------------------------------------------------------===//

// Force static initialization.
extern "C" void LLVMInitializeAVRAsmPrinter()
{
  RegisterAsmPrinter<AVRAsmPrinter> X(TheAVRTarget);
}
