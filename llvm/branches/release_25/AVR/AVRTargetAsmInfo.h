//=====-- AVRTargetAsmInfo.h - AVR asm properties ---------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the AVRTargetAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef AVRTARGETASMINFO_H
#define AVRTARGETASMINFO_H

#include "llvm/Target/TargetAsmInfo.h"

namespace llvm {

  // Forward declaration.
  class AVRTargetMachine;

  struct AVRTargetAsmInfo : public TargetAsmInfo {
    AVRTargetAsmInfo(const AVRTargetMachine &TM);
    private:
    /*const char *RomData8bitsDirective;
    const char *RomData16bitsDirective;
    const char *RomData32bitsDirective;
    const char *getRomDirective(unsigned size) const;
    virtual const char *getASDirective(unsigned size, unsigned AS) const;*/
  };

} // namespace llvm

#endif
