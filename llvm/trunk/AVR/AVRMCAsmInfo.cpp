//===------ AVRMCAsmInfo.cpp - AVR asm properties -------------------------===//
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

AVRELFMCAsmInfo::AVRELFMCAsmInfo(const Target &T, StringRef TT)
{
  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";

  PCSymbol = ".";
  SeparatorString = "$";
  GlobalDirective = "\t.global\t";
  ExternDirective = "\t.extern\t";
  WeakRefDirective = "\t.weakref\t";    // .weakref alias, target

  HasLCOMMDirective = true;             // static variables == .lcomm
  UsesELFSectionDirectiveForBSS = true;

  //:TODO: implement all this
}
