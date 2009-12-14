//=====-- AVRMCAsmInfo.h - AVR asm properties ---------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the AVRMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef AVRTARGETASMINFO_H
#define AVRTARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm
{
  class Target;
  class StringRef;

  struct AVRMCAsmInfo : public MCAsmInfo
  {
    explicit AVRMCAsmInfo(const Target &T, const StringRef &TT);
    
    /// Get Data Directive (address space specific)
    virtual const char *getDataASDirective(unsigned Size, unsigned AS) const
    {
      assert(AS != 0 && "Don't know the directives for default addr space");
      return 0;
    }
    
  };

} // namespace llvm

#endif
