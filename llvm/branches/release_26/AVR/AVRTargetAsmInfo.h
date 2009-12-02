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
  class Target;
  class StringRef;

  struct AVRTargetAsmInfo : public TargetAsmInfo {
    explicit AVRTargetAsmInfo(const Target &T, const StringRef &TT);
  };

} // namespace llvm

#endif
