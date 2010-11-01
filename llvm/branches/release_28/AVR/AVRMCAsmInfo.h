//=====-------- AVRMCAsmInfo.h - AVR asm properties -----------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the AVRMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef __INCLUDE_AVRMCASMINFO_H__
#define __INCLUDE_AVRMCASMINFO_H__

#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCAsmInfo.h"

namespace llvm
{

class Target;

struct AVRELFMCAsmInfo : public MCAsmInfo
{
  explicit AVRELFMCAsmInfo(const Target &T, StringRef TT);
};

} // namespace llvm

#endif // __INCLUDE_AVRMCASMINFO_H__
