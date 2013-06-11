//===-- AVRMCAsmInfo.cpp - AVR asm properties -----------------------------===//
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
#include "llvm/ADT/StringRef.h"

using namespace llvm;

void AVRMCAsmInfo::anchor() { }

AVRMCAsmInfo::AVRMCAsmInfo(const Target &T, StringRef TT)
{
  PointerSize = 2;

  PCSymbol = ".";
  CommentString = ";";
  PrivateGlobalPrefix = ".L";

  UsesELFSectionDirectiveForBSS = true;
}
