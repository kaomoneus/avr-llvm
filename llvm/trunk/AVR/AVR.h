//===------------ AVR.h - Top-level interface for AVR -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM AVR back-end.
//
//===----------------------------------------------------------------------===// 

#ifndef __INCLUDE_AVR_H__
#define __INCLUDE_AVR_H__

#include "llvm/Target/TargetMachine.h"

namespace llvm
{
  class FunctionPass;
  class AVRTargetMachine;

  FunctionPass *createAVRISelDag(AVRTargetMachine &TM,
                                 CodeGenOpt::Level OptLevel);

  FunctionPass *createAVR16bitInstPass();

  extern Target TheAVRTarget;
} // end namespace llvm

// Defines symbolic names for AVR registers.  This defines a mapping from
// register name to register number.
//
#include "AVRGenRegisterNames.inc"

// Defines symbolic names for the AVR instructions.
//
#include "AVRGenInstrNames.inc"

#endif // __INCLUDE_AVR_H__
