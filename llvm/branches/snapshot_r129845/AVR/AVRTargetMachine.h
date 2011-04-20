//===--   AVRTargetMachine.h - Define TargetMachine for AVR   ---*- C++ -*-===//
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

#ifndef __INCLUDE_AVRTARGETMACHINE_H__
#define __INCLUDE_AVRTARGETMACHINE_H__

#include "AVRInstrInfo.h"
#include "AVRISelLowering.h"
#include "AVRFrameLowering.h"
#include "AVRSelectionDAGInfo.h"
#include "AVRRegisterInfo.h"
#include "AVRSubtarget.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm
{

class AVRTargetMachine : public LLVMTargetMachine
{
public:
  AVRTargetMachine(const Target &T,
                   const std::string &TT,
                   const std::string &FS);
public: // TargetMachine
  const AVRInstrInfo *getInstrInfo() const;
  const AVRTargetLowering *getTargetLowering() const;
  const TargetData *getTargetData() const;
  const AVRRegisterInfo *getRegisterInfo() const;
  const AVRSubtarget *getSubtargetImpl() const;
  const TargetFrameLowering *getFrameInfo() const;
  const AVRSelectionDAGInfo *getSelectionDAGInfo() const;
  bool addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
  bool addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
private:
  AVRSubtarget Subtarget;
  const TargetData DataLayout;
  AVRInstrInfo InstrInfo;
  AVRTargetLowering TLInfo;
  AVRSelectionDAGInfo TSInfo;
  AVRFrameLowering FrameLowering;
};

} // end namespace llvm

#endif //__INCLUDE_AVRTARGETMACHINE_H__
