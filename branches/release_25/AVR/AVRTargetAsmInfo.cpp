//===-- AVRTargetAsmInfo.cpp - AVR asm properties ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the AVRTargetAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "AVRTargetAsmInfo.h"
#include "AVRTargetMachine.h"
#include "llvm/GlobalValue.h"

using namespace llvm;

AVRTargetAsmInfo::
AVRTargetAsmInfo(const AVRTargetMachine &TM) 
  : TargetAsmInfo(TM) {
  CommentString = ";";
//  Data8bitsDirective = " db ";
//  Data16bitsDirective = " dw ";
//  Data32bitsDirective = " dl ";
//  RomData8bitsDirective = " dw ";
//  RomData16bitsDirective = " rom_di ";
//  RomData32bitsDirective = " rom_dl ";
//  ZeroDirective = NULL;
//  AsciiDirective = " dt ";
//  AscizDirective = NULL;
//  BSSSection_  = getNamedSection("udata.# UDATA",
//                              SectionFlags::Writeable | SectionFlags::BSS);
//  ReadOnlySection = getNamedSection("romdata.# ROMDATA", SectionFlags::None);
//  DataSection = getNamedSection("idata.# IDATA", SectionFlags::Writeable);
//  SwitchToSectionDirective = "";
}
/*
const char *AVRTargetAsmInfo::getRomDirective(unsigned size) const
{
  if (size == 8)
    return RomData8bitsDirective;
  else if (size == 16)
    return RomData16bitsDirective;
  else if (size == 32)
    return RomData32bitsDirective;
  else
    return NULL;
}


const char *AVRTargetAsmInfo::getASDirective(unsigned size, 
                                               unsigned AS) const {
  if (AS == AVRISD::ROM_SPACE)
    return getRomDirective(size);
  else
    return NULL;
}
*/
