//===-- AVRTargetMachine.cpp - Define TargetMachine for AVR ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the AVR specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "AVRTargetMachine.h"
#include "AVR.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

extern "C" void LLVMInitializeAVRTarget()
{
  // Register the target.
  RegisterTargetMachine<AVRTargetMachine> X(TheAVRTarget);
}

AVRTargetMachine::AVRTargetMachine(const Target &T, StringRef TT, StringRef CPU,
                                   StringRef FS, const TargetOptions &Options,
                                   Reloc::Model RM, CodeModel::Model CM,
                                   CodeGenOpt::Level OL) :
  LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
  SubTarget(TT, CPU, FS),
  DataLayout("e-p:16:8:8-i8:8:8-i16:8:8-i32:8:8-i64:8:8-f32:8:8-f64:8:8-n8"),
  InstrInfo(),
  FrameLowering(),
  TLInfo(*this),
  TSInfo(*this) {}

namespace
{
/// AVR Code Generator Pass Configuration Options.
class AVRPassConfig : public TargetPassConfig
{
public:
  AVRPassConfig(AVRTargetMachine *TM, PassManagerBase &PM) :
    TargetPassConfig(TM, PM) {}

  AVRTargetMachine &getAVRTargetMachine() const
  {
    return getTM<AVRTargetMachine>();
  }

  bool addInstSelector();
  bool addPreSched2();
};
} // namespace

TargetPassConfig *AVRTargetMachine::createPassConfig(PassManagerBase &PM)
{
  return new AVRPassConfig(this, PM);
}

const AVRSubtarget *AVRTargetMachine::getSubtargetImpl() const
{
  return &SubTarget;
}

const AVRInstrInfo *AVRTargetMachine::getInstrInfo() const
{
  return &InstrInfo;
}

const TargetFrameLowering *AVRTargetMachine::getFrameLowering() const
{
  return &FrameLowering;
}

const AVRTargetLowering *AVRTargetMachine::getTargetLowering() const
{
  return &TLInfo;
}

const AVRSelectionDAGInfo *AVRTargetMachine::getSelectionDAGInfo() const
{
  return &TSInfo;
}

const TargetData *AVRTargetMachine::getTargetData() const
{
  return &DataLayout;
}

const AVRRegisterInfo *AVRTargetMachine::getRegisterInfo() const
{
  return &InstrInfo.getRegisterInfo();
}

//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

bool AVRPassConfig::addInstSelector()
{
  // Install an instruction selector.
  addPass(createAVRISelDag(getAVRTargetMachine(), getOptLevel()));
  addPass(createAVRFrameAnalyzerPass());
  return false;
}

bool AVRPassConfig::addPreSched2()
{
  addPass(createAVRExpandPseudoPass());
  return true;
}

static FunctionPass *createAVRRegisterAllocator()
{
  return createGreedyRegisterAllocator();
}

//:FIXME: make this the default regalloc instead of forcing users to specify
// it in the cmd line
static RegisterRegAlloc avrRegAlloc("avrgreedy",
                                    "AVR greedy custom register allocator",
                                    createAVRRegisterAllocator);
