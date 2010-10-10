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
  //:TODO: implement all this
}
