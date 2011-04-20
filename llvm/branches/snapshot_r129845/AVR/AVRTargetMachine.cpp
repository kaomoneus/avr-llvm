//===------ AVRTargetMachine.cpp - Define TargetMachine for the AVR -------===//
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

#include "AVR.h"
#include "AVRTargetMachine.h"
#include "AVRRegisterInfo.h"
#include "AVRMCAsmInfo.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/PassManager.h"

using namespace llvm;

extern "C" void LLVMInitializeAVRTarget()
{
  RegisterTargetMachine<AVRTargetMachine> X(TheAVRTarget);
  RegisterAsmInfo<AVRELFMCAsmInfo> Y(TheAVRTarget);
}

//datalayout string
//:TODO: see if type widths match avr libc ABI
// "e-p:16:8:8-i8:8:8-i16:8:8-i32:8:8-i64:8:8-f32:8:8-f64:8:8-n8"
//:TODO: CHECK TARGET ENDIANNESS!!!!
AVRTargetMachine::AVRTargetMachine(const Target &T, const std::string &TT,
                                   const std::string &FS) :
  LLVMTargetMachine(T, TT),
  Subtarget(TT, FS),
  DataLayout("E-p:16:8:8-i8:8:8-i16:8:8-i32:8:8-i64:8:8-f32:8:8-f64:8:8-n8"),
  InstrInfo(),
  TLInfo(*this),
  TSInfo(*this),
  FrameLowering(Subtarget) {}

const AVRInstrInfo *AVRTargetMachine::getInstrInfo() const
{
  return &InstrInfo;
}

const AVRTargetLowering *AVRTargetMachine::getTargetLowering() const
{
  return &TLInfo;
}

const TargetData *AVRTargetMachine::getTargetData() const
{
  return &DataLayout;
}

const AVRRegisterInfo *AVRTargetMachine::getRegisterInfo() const
{
  return &(InstrInfo.getRegisterInfo());
}

const AVRSubtarget *AVRTargetMachine::getSubtargetImpl() const
{
  return &Subtarget;
}

const TargetFrameLowering *AVRTargetMachine::getFrameInfo() const
{
  return &FrameLowering;
}

const AVRSelectionDAGInfo *AVRTargetMachine::getSelectionDAGInfo() const
{
  return &TSInfo;
}

bool AVRTargetMachine::addInstSelector(PassManagerBase &PM,
                                       CodeGenOpt::Level OptLevel)
{
  PM.add(createAVRISelDag(*this, OptLevel));

  return false;
}

bool AVRTargetMachine::addPreEmitPass(PassManagerBase &PM,
                                      CodeGenOpt::Level OptLevel)
{
  //PM.add(createAVR16bitInstPass()); //:TODO: temporarily disabled

  return false;
}

