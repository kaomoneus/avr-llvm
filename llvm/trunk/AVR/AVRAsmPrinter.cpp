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
