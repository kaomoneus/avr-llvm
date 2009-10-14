//===-- AVRTargetMachine.h - Define TargetMachine for AVR ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the AVR specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//


#ifndef AVR_TARGETMACHINE_H
#define AVR_TARGETMACHINE_H

#include "AVRInstrInfo.h"
#include "AVRISelLowering.h"
#include "AVRFrameInfo.h"
#include "AVRRegisterInfo.h"
#include "AVRSubtarget.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

/// AVRTargetMachine
///
class AVRTargetMachine : public LLVMTargetMachine {
  AVRSubtarget        Subtarget;
  const TargetData   DataLayout;       // Calculates type size & alignment
  AVRInstrInfo        InstrInfo;
  AVRTargetLowering   TLInfo;

  AVRFrameInfo        FrameInfo;

public:
  AVRTargetMachine(const Target &T, const std::string &TT, const std::string &FS);

  virtual const TargetFrameInfo *getFrameInfo() const { return &FrameInfo; }
  virtual const AVRInstrInfo *getInstrInfo() const  { return &InstrInfo; }
  virtual const TargetData *getTargetData() const     { return &DataLayout;}
  virtual const AVRSubtarget *getSubtargetImpl() const { return &Subtarget; }

  virtual const AVRRegisterInfo *getRegisterInfo() const {
    return &(InstrInfo.getRegisterInfo());
  }

  virtual AVRTargetLowering *getTargetLowering() const {
    return const_cast<AVRTargetLowering*>(&TLInfo);
  }

  virtual bool addInstSelector(PassManagerBase &PM,
                               CodeGenOpt::Level OptLevel);
  virtual bool addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
}; // AVRTargetMachine.

} // end namespace llvm

#endif
