//===- AVRSubtarget.cpp - AVR Subtarget Information -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the AVR specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "AVRSubtarget.h"
#include "AVRGenSubtarget.inc"

using namespace llvm;

AVRSubtarget::AVRSubtarget(const std::string &TT, const std::string &FS) {

  std::string CPU = "generic";

  // Parse features string.
  ParseSubtargetFeatures(FS, CPU);
}
