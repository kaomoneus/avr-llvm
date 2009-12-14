//===-- AVRMCAsmInfo.cpp - AVR asm properties ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the AVRMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "AVRMCAsmInfo.h"

using namespace llvm;

AVRMCAsmInfo::AVRMCAsmInfo(const Target &T, const StringRef &TT)
{
  PCSymbol = ".";
  SeparatorChar = '$';
  SetDirective = "\t.set\t";
  LCOMMDirective = "\t.lcomm\t";
  GlobalDirective = "\t.global\t";
  ExternDirective = "\t.extern\t";
  UsesELFSectionDirectiveForBSS = true;
}
