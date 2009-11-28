//===- AVRMachineFuctionInfo.h - AVR machine function info -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares AVR-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef AVRMACHINEFUNCTIONINFO_H
#define AVRMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// AVRMachineFunctionInfo - This class is derived from MachineFunction and
/// contains private AVR target-specific information for each MachineFunction.
class AVRMachineFunctionInfo : public MachineFunctionInfo {
  /// CalleeSavedFrameSize - Size of the callee-saved register portion of the
  /// stack frame in bytes.
  unsigned CalleeSavedFrameSize;

public:
  AVRMachineFunctionInfo() : CalleeSavedFrameSize(0) {}

  explicit AVRMachineFunctionInfo(MachineFunction &MF)
    : CalleeSavedFrameSize(0) {}

  unsigned getCalleeSavedFrameSize() const { return CalleeSavedFrameSize; }
  void setCalleeSavedFrameSize(unsigned bytes) { CalleeSavedFrameSize = bytes; }
};

} // End llvm namespace

#endif
