//===-- AVRAsmPrinter.cpp - AVR LLVM assembly writer ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AVR assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "AVR.h"
//#include "AVRInstrInfo.h"
#include "InstPrinter/AVRInstPrinter.h"
#include "AVRMCAsmInfo.h"
#include "AVRMCInstLower.h"
#include "AVRTargetMachine.h"
#include "llvm/LLVMContext.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace
{
  class AVRAsmPrinter : public AsmPrinter
  {
    public:
      explicit AVRAsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
        : AsmPrinter(TM, Streamer) {}

      virtual const char *getPassName() const
      {
        return "AVR Assembly Printer";
      }
      virtual void EmitStartOfAsmFile(Module &M)
      {
        M.setModuleInlineAsm ("__SREG__ = 0x3f\n"
                              "__SP_H__ = 0x3e\n"
                              "__SP_L__ = 0x3d\n"
                              "__CCP__  = 0x34\n"
                              "__tmp_reg__ = 0\n"
                              "__zero_reg__ = 1");
      }

      void printOperand(const MachineInstr *MI, int OpNum,
                      raw_ostream &O, const char* Modifier = 0);
      void EmitInstruction(const MachineInstr *MI);

  };
} // end of anonymous namespace


void AVRAsmPrinter::printOperand(const MachineInstr *MI, int OpNum,
                                    raw_ostream &O, const char *Modifier)
{
  const MachineOperand &MO = MI->getOperand(OpNum);
  switch (MO.getType()) 
  {
    case MachineOperand::MO_Register:
	     O << AVRInstPrinter::getRegisterName(MO.getReg());
	     return;
    case MachineOperand::MO_Immediate:
      O << MO.getImm();
      return;
    case MachineOperand::MO_MachineBasicBlock:
      O << *MO.getMBB()->getSymbol();
      return;
    case MachineOperand::MO_GlobalAddress:
      O << *Mang->getSymbol(MO.getGlobal());
      return;
    case MachineOperand::MO_ExternalSymbol:
      O << MO.getSymbolName();
      return;
    default:
      llvm_unreachable("Not implemented yet!");
  }
}

//===----------------------------------------------------------------------===//
void AVRAsmPrinter::EmitInstruction(const MachineInstr *MI)
{

  AVRMCInstLower MCInstLowering(OutContext, *Mang, *this);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst);
}

static MCInstPrinter *createAVRMCInstPrinter(const Target &T,
                                              TargetMachine &TM,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI) 
{
  if (SyntaxVariant == 0)
    return new AVRInstPrinter(TM, MAI);
  return 0;
}


// Force static initialization.
extern "C" void LLVMInitializeAVRAsmPrinter()
{
  RegisterAsmPrinter<AVRAsmPrinter> X(TheAVRTarget);
  TargetRegistry::RegisterMCInstPrinter(TheAVRTarget,
                                        createAVRMCInstPrinter);
}
