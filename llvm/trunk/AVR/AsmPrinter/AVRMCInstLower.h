//===-- AVRMCInstLower.h - Lower MachineInstr to MCInst ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef AVR_MCINSTLOWER_H
#define AVR_MCINSTLOWER_H

#include "llvm/Support/Compiler.h"

namespace llvm {
  class AsmPrinter;
  class MCAsmInfo;
  class MCContext;
  class MCInst;
  class MCOperand;
  class MCSymbol;
  class MachineInstr;
  class MachineModuleInfoMachO;
  class MachineOperand;
  class Mangler;

  /// AVRMCInstLower - This class is used to lower an MachineInstr
  /// into an MCInst.
  class VISIBILITY_HIDDEN AVRMCInstLower
  {
    MCContext &Ctx;
    Mangler &Mang;

    AsmPrinter &Printer;
    public:
      AVRMCInstLower(MCContext &ctx, Mangler &mang, AsmPrinter &printer)
        : Ctx(ctx), Mang(mang), Printer(printer) {}
      void Lower(const MachineInstr *MI, MCInst &OutMI) const;

      MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

      MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
      MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;
      MCSymbol *GetJumpTableSymbol(const MachineOperand &MO) const;
      MCSymbol *GetConstantPoolIndexSymbol(const MachineOperand &MO) const;
  };

}

#endif
