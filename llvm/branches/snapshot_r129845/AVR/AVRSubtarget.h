//===------ AVRSubtarget.h - Define Subtarget for the AVR ------*- C++ -*-====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the AVR specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#ifndef __INCLUDE_AVRSUBTARGET_H__
#define __INCLUDE_AVRSUBTARGET_H__

#include <string>
#include "llvm/Target/TargetSubtarget.h"

namespace llvm
{

class AVRSubtarget : public TargetSubtarget
{
public:
  AVRSubtarget(const std::string &TT, const std::string &FS);
public:
  std::string ParseSubtargetFeatures(const std::string &FS,
                                     const std::string &CPU);
private:
  bool HasJMP;
  bool HasMUL;
  bool HasMOVW;
  bool HasJTAG;
  bool HasEIJMP;
  bool HasSPM;
  bool HasDES;
  bool HasEIJMP_EICALL;
  bool HasELPM;
  bool HasELPMX;
  bool HasJMP_CALL;
  bool HasMOVW_LPMX;
  bool HasRAMPX_Y_D;
  bool HasTiny_Regs_Insns;
  bool HasCoreExt;

  bool IsAsmOnly;
  bool IsAVRTINY1;
  bool IsAVRXMEGA1;
  bool IsAVRXMEGA2;
  bool IsAVRXMEGA3;
  bool IsAVRXMEGA4;
  bool IsAVRXMEGA5;
  bool IsAVRXMEGA6;
  bool IsAVRXMEGA7;

  bool IsAVR1;
  bool IsAVR2;
  bool IsAVR25;
  bool IsAVR3;
  bool IsAVR31;
  bool IsAVR35;
  bool IsAVR4;
  bool IsAVR5;
  bool IsAVR51;
  bool IsAVR6;
  bool IsXMEGA;
};

} // end namespace llvm

#endif // __INCLUDE_AVRSUBTARGET_H__
