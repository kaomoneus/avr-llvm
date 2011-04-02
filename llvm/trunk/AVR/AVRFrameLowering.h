//===-- AVRFrameLowering.h  - Define Frame Lowering  for AVR  --*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef AVR_FRAMEINFO_H
#define AVR_FRAMEINFO_H

#include "AVR.h"
#include "AVRSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class AVRSubtarget;

class AVRFrameLowering : public TargetFrameLowering {
protected:
  const AVRSubtarget &STI;

public:
  explicit AVRFrameLowering(const AVRSubtarget &sti)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 8, 0), STI(sti) {
  }

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  bool hasFP(const MachineFunction &MF) const;
};

} // End llvm namespace

#endif

