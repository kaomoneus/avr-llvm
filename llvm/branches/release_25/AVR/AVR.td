//===- AVR.td - Describe the AVR Target Machine ---------------*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the AVR target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Descriptions
//===----------------------------------------------------------------------===//

include "AVRRegisterInfo.td"
//include "AVRCallingConv.td"
include "AVRInstrInfo.td"

def AVRInstrInfo : InstrInfo {
  let TSFlagsFields = [];
  let TSFlagsShifts = [];
}

//===----------------------------------------------------------------------===//
// AVR subtarget features.
//===----------------------------------------------------------------------===//

// up to 128K flash, direct jump
def FeatureJMP
  : SubtargetFeature<"jmp","HasJMP", "true",
                     "Enable Direct Jump instructions">;
// Mul instructions (MUL, FMUL)
def FeatureMUL
  : SubtargetFeature<"mul","HasMUL", "true",
                     "Enable Mul instructions">; 
// MOVW instructions 
def FeatureMOVW
  : SubtargetFeature<"movw","HasMOVW", "true",
                     "Enable MOVW instructions">; 
// Enable JTAG instructions (break)
def FeatureJTAG
  : SubtargetFeature<"jtag","HasJTAG", "true",
                     "Enable JTAG instructions">;
// up to 4M Flash (EIJMP, EICALL)
def FeatureEIJMP
  : SubtargetFeature<"eijmp", "HasEIJMP", "true",
                     "Enable EIJMP instructions">;
// Self Write
def FeatureSPM
  : SubtargetFeature<"spm", "HasSPM", "true",
                     "Enable Self Write instructions">;
// DES Data Encryption Standard
def FeatureDES
  : SubtargetFeature<"des", "HasDES", "true",
                     "Enable DES instruction">;


// generic feature sets

// Classic AVR Core (default)
def FeatureAVR2
  : SubtargetFeature<"avr2", "IsAVR2", "true",
                     "Enable AVR2 instructions",
                     []>;
// Classic AVR Core with MOVW instruction
def FeatureAVR25
  : SubtargetFeature<"avr25", "IsAVR25", "true",
                     "Enable AVR25 instructions",
                     [FeatureMOVW]>;
// Classic AVR Core with up to 128K flash 
def FeatureAVR3
  : SubtargetFeature<"avr3", "IsAVR3", "true",
                     "Enable AVR3 instructions",
                     []>;
// Classic AVR Core with exactly 128K flash 
def FeatureAVR31
  : SubtargetFeature<"avr31", "IsAVR31", "true",
                     "Enable AVR31 instructions",
                     [FeatureAVR3,FeatureJMP]>;
// Enhanced AVR Core with up to 8K flash 
def FeatureAVR4
  : SubtargetFeature<"avr4", "IsAVR4", "true",
                     "Enable AVR4 instructions",
                     [FeatureMUL,FeatureMOVW,FeatureSPM]>;
// Enhanced AVR Core with up to 128K flash 
def FeatureAVR5
  : SubtargetFeature<"avr5", "IsAVR5", "true",
                     "Enable AVR5 instructions",
                     [FeatureAVR4,FeatureJTAG]>;
// Enhanced AVR Core with exactly 128K flash 
def FeatureAVR51
  : SubtargetFeature<"avr51", "IsAVR51", "true",
                     "Enable AVR51 instructions",
                     [FeatureAVR5,FeatureJMP]>;
// Enhanced AVR Core with 3-byte PC (4M Flash) 
def FeatureAVR6
  : SubtargetFeature<"avr6", "IsAVR6", "true",
                     "Enable AVR6 instructions",
                     [FeatureAVR51,FeatureEIJMP]>;
// AVR XMEGA Core 
def FeatureXMEGA
  : SubtargetFeature<"xmega", "IsXMEGA", "true",
                     "Enable XMEGA instructions",
                     [FeatureAVR6,FeatureDES]>;

//===----------------------------------------------------------------------===//
// AVR processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic",   []>;
def : Proc<"avr2",      []>;
def : Proc<"avr25",     [FeatureAVR25]>;
def : Proc<"avr3",      [FeatureAVR3]>;
def : Proc<"avr31",     [FeatureAVR3]>;
def : Proc<"avr4",      [FeatureAVR4]>;
def : Proc<"avr5",      [FeatureAVR5]>;
def : Proc<"avr51",     [FeatureAVR51]>;
def : Proc<"avr6",      [FeatureAVR6]>;
def : Proc<"xmega",     [FeatureXMEGA]>;

def AVR : Target {
  let InstructionSet = AVRInstrInfo;
}

