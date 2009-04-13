//===-- AVRTargetMachine.h - Define TargetMachine for AVR -------*- C++ -*-===//
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


#ifndef AVR_TARGETMACHINE_H
#define AVR_TARGETMACHINE_H

#include "AVRInstrInfo.h"
#include "AVRISelLowering.h"
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
  const TargetData      DataLayout;       // Calculates type size & alignment
  AVRInstrInfo        InstrInfo;
  AVRTargetLowering   TLInfo;
  TargetFrameInfo       FrameInfo;

protected:
  virtual const TargetAsmInfo *createTargetAsmInfo() const;
  
public:
  AVRTargetMachine(const Module &M, const std::string &FS);

  virtual const TargetFrameInfo *getFrameInfo() const 
  { return &FrameInfo; }
  virtual const AVRInstrInfo *getInstrInfo() const 
  { return &InstrInfo; }
  virtual const TargetData *getTargetData() const    
  { return &DataLayout; }
  virtual AVRTargetLowering *getTargetLowering() const 
  { return const_cast<AVRTargetLowering*>(&TLInfo); }
  virtual const AVRRegisterInfo *getRegisterInfo() const 
  { return &InstrInfo.getRegisterInfo(); }
  virtual const AVRSubtarget *getSubtargetImpl() const 
  { return &Subtarget; }  
  virtual bool addInstSelector(PassManagerBase &PM, bool Fast);
  virtual bool addPrologEpilogInserter(PassManagerBase &PM, bool Fast);
  virtual bool addPreEmitPass(PassManagerBase &PM, bool Fast);
  virtual bool addAssemblyEmitter(PassManagerBase &PM, bool Fast, 
                                  raw_ostream &Out);
};
} // end namespace llvm

#endif
