//===-- AVRTargetMachine.cpp - Define TargetMachine for AVR -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top-level implementation for the AVR target.
//
//===----------------------------------------------------------------------===//

#include "AVR.h"
#include "AVRTargetMachine.h"
#include "AVRMCAsmInfo.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
//#include "llvm/Target/TargetAsmInfo.h"
#include "llvm/Target/TargetRegistry.h"

using namespace llvm;

#if 0
static const TargetAsmInfo *createTargetAsmInfo(const Target &T,
                                                const StringRef &TT) {
  Triple TheTriple(TT);

  return new AVRTargetAsmInfo(T, TT);

}
#endif
extern "C" void LLVMInitializeAVRTarget() {
  // Register the target.
  RegisterTargetMachine<AVRTargetMachine> X(TheAVRTarget);
  RegisterAsmInfo<AVRMCAsmInfo> A(TheAVRTarget);

}


// Constructor
AVRTargetMachine::AVRTargetMachine(const Target &T, const std::string &TT,
                                       const std::string &FS)
  : LLVMTargetMachine(T, TT),
    Subtarget(TT, FS),
    DataLayout("e-p:16:8:8-i8:8:8-i16:8:8-i32:8:8"),
    InstrInfo(Subtarget),
    TLInfo(*this),
    FrameInfo(TargetFrameInfo::StackGrowsDown, 1, 0) { }//FrameInfo()

bool AVRTargetMachine::addInstSelector(PassManagerBase &PM,
                                         CodeGenOpt::Level OptLevel) {
  // Install an instruction selector.
  PM.add(createAVRISelDag(*this, OptLevel));
  return false;
}
/*
bool AVRTargetMachine::addPreEmitPass(PassManagerBase &PM,
                                         CodeGenOpt::Level OptLevel) {
  //PM.add(createAVRMemSelOptimizerPass());
  return true;  // -print-machineinstr should print after this.
}
*/

