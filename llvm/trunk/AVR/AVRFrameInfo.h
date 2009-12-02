//===-- AVRTargetFrameInfo.h - Define TargetFrameInfo for AVR ---*- C++ -*-===//
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
#include "llvm/Target/TargetFrameInfo.h"

namespace llvm {

class AVRFrameInfo : public TargetFrameInfo {

  public:
    AVRFrameInfo(/*const TargetMachine &tm*/)
		:  TargetFrameInfo(StackGrowsDown, 1, 0) {}
};

} // End llvm namespace

#endif
