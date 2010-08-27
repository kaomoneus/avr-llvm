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
include "AVRCallingConv.td"
include "AVRInstrInfo.td"


def AVRInstrInfo : InstrInfo
{
//  let TSFlagsFields = [];
//  let TSFlagsShifts = [];
}

def AVRInstPrinter : AsmWriter 
{
  string AsmWriterClassName  = "InstPrinter";
}

//===----------------------------------------------------------------------===//
// AVR subtarget features.
//===----------------------------------------------------------------------===//

def FeatureAsmOnly
  : SubtargetFeature<"asmonly","IsAsmOnly", "true", 
                     "Device is assembler only">;

// Multiply instructions (MUL, FMUL)
def FeatureMUL
  : SubtargetFeature<"mul","HasMUL", "true",
                     "Device has multiply instructions">; 

// Up to 128K flash, direct jump.
///\todo seperating these would add more flexibility when using -mattr=
def FeatureJMP_CALL
  : SubtargetFeature<"jmp-call","HasJMP_CALL", "true",
                     "Device has JMP and CALL instructions">;

// MOVW and LPMX instructions
///\todo seperating these would add more flexibility when using -mattr=
def FeatureMOVW_LPMX
  : SubtargetFeature<"movw-lpmx","HasMOVW_LPMX", "true",
                     "Device has MOVW and LPM Rx,Z instructions">; 

// ELPM instruction.
def FeatureELPM
  : SubtargetFeature<"elpm","HasELPM", "true",
                     "Device has ELPM instruction">; 

// ELPMX instruction.
def FeatureELPMX
  : SubtargetFeature<"elpmx","HasELPMX", "true",
                     "Device has ELPM Rx,Z instruction">; 

// Up to 4M Flash (EIJMP, EICALL)
def FeatureEIJMP_EICALL
  : SubtargetFeature<"eijmp-eicall", "HasEIJMP_EICALL", "true",
                     "Device has EIJMP and EICALL instructions">;

def FeatureXMEGA
  : SubtargetFeature<"xmega", "IsXMEGA", "true",
                     "Device is in the XMEGA family">;
                     
def FeatureRAMPX_Y_D
  : SubtargetFeature<"rampx-y-d", "HasRAMPX_Y_D", "true",
                     "Device has RAMPX, RAMPY, and RAMPD registers">;

def FeatureTiny_Regs_Insns
  : SubtargetFeature<"tiny-regs-insns", "HasTiny_Regs_Insns", "true",
                     "Device has Tiny registers and instructions">;

def FeatureCoreExt
  : SubtargetFeature<"core-ext", "HasCoreExt", "true",
                     "Device has a core extension">;

// Feature Sets

// Assembler only.
def FeatureAVR1
  : SubtargetFeature<"avr1", "IsAVR1", "true",
                     "AVR1 architecture",
                     [FeatureAsmOnly]>;

// Classic AVR Core (default)
def FeatureAVR2
  : SubtargetFeature<"avr2", "IsAVR2", "true",
                     "AVR2 architecture",
                     []>;

// Classic AVR Core with MOVW instruction
def FeatureAVR25
  : SubtargetFeature<"avr25", "IsAVR25", "true",
                     "AVR25 architecture",
                     [FeatureMOVW_LPMX]>;

// Classic AVR Core with up to 128K flash 
def FeatureAVR3
  : SubtargetFeature<"avr3", "IsAVR3", "true",
                     "AVR3 architecture",
                     [FeatureJMP_CALL]>;

// Classic AVR Core with exactly 128K flash 
def FeatureAVR31
  : SubtargetFeature<"avr31", "IsAVR31", "true",
                     "AVR31 architecture",
                     [FeatureJMP_CALL, FeatureELPM]>;

// Classic AVR Core with up to 128K flash 
def FeatureAVR35
  : SubtargetFeature<"avr35", "IsAVR35", "true",
                     "AVR35 architecture",
                     [FeatureJMP_CALL, FeatureMOVW_LPMX]>;

// Enhanced AVR Core with up to 8K flash 
def FeatureAVR4
  : SubtargetFeature<"avr4", "IsAVR4", "true",
                     "AVR4 architecture",
                     [FeatureMUL, FeatureMOVW_LPMX]>;

// Enhanced AVR Core with up to 128K flash 
def FeatureAVR5
  : SubtargetFeature<"avr5", "IsAVR5", "true",
                     "AVR5 architecture",
                     [FeatureAVR4, FeatureJMP_CALL]>;

// Enhanced AVR Core with exactly 128K flash 
def FeatureAVR51
  : SubtargetFeature<"avr51", "IsAVR51", "true",
                     "AVR51 architecture",
                     [FeatureAVR5, FeatureELPM, FeatureELPMX]>;

// Enhanced AVR Core with 3-byte PC (4M Flash) 
def FeatureAVR6
  : SubtargetFeature<"avr6", "IsAVR6", "true",
                     "AVR6 architecture",
                     [FeatureAVR51, FeatureEIJMP_EICALL]>;

def FeatureAVRXMEGA1
  : SubtargetFeature<"avrxmega1", "IsAVRXMEGA1", "true",
                     "AVRXMEGA1 architecture",
                     [FeatureMUL, FeatureMOVW_LPMX, FeatureXMEGA]>;

def FeatureAVRXMEGA2
  : SubtargetFeature<"avrxmega2", "IsAVRXMEGA2", "true",
                     "AVRXMEGA2 architecture",
                     [FeatureAVRXMEGA1, FeatureJMP_CALL]>;

def FeatureAVRXMEGA3
  : SubtargetFeature<"avrxmega3", "IsAVRXMEGA3", "true",
                     "AVRXMEGA3 architecture",
                     [FeatureAVRXMEGA2, FeatureRAMPX_Y_D]>;

def FeatureAVRXMEGA4
  : SubtargetFeature<"avrxmega4", "IsAVRXMEGA4", "true",
                     "AVRXMEGA4 architecture",
                     [FeatureAVRXMEGA2, FeatureELPM, FeatureELPMX]>;

def FeatureAVRXMEGA5
  : SubtargetFeature<"avrxmega5", "IsAVRXMEGA5", "true",
                     "AVRXMEGA5 architecture",
                     [FeatureAVRXMEGA4, FeatureRAMPX_Y_D]>;

def FeatureAVRXMEGA6
  : SubtargetFeature<"avrxmega6", "IsAVRXMEGA6", "true",
                     "AVRXMEGA6 architecture",
                     [FeatureAVRXMEGA4, FeatureEIJMP_EICALL]>;

def FeatureAVRXMEGA7
  : SubtargetFeature<"avrxmega7", "IsAVRXMEGA7", "true",
                     "AVRXMEGA7 architecture",
                     [FeatureAVRXMEGA6, FeatureRAMPX_Y_D]>;

def FeatureAVRTINY1
  : SubtargetFeature<"avrtiny1", "IsAVRTINY1", "true",
                     "AVRTINY1 architecture",
                     [FeatureTiny_Regs_Insns]>;


//===----------------------------------------------------------------------===//
// AVR devices supported.
//===----------------------------------------------------------------------===//

class Device <string Name, list<SubtargetFeature> Features> :
    Processor <Name, NoItineraries, Features>;

// Assembler only.
def : Device<"avr1",         [FeatureAVR1] >;
def : Device<"at90s1200",    [FeatureAVR1] >; // "__AVR_AT90S1200__"
def : Device<"attiny11",     [FeatureAVR1] >; // "__AVR_ATtiny11__"
def : Device<"attiny12",     [FeatureAVR1] >; // "__AVR_ATtiny12__"
def : Device<"attiny15",     [FeatureAVR1] >; // "__AVR_ATtiny15__"
def : Device<"attiny28",     [FeatureAVR1] >; // "__AVR_ATtiny28__"

def : Device<"avr2",         [FeatureAVR2] >;
def : Device<"at90s2313",    [FeatureAVR2] >; // "__AVR_AT90S2313__"
def : Device<"at90s2323",    [FeatureAVR2] >; // "__AVR_AT90S2323__"
def : Device<"at90s2333",    [FeatureAVR2] >; // "__AVR_AT90S2333__"
def : Device<"at90s2343",    [FeatureAVR2] >; // "__AVR_AT90S2343__"
def : Device<"attiny22",     [FeatureAVR2] >; // "__AVR_ATtiny22__"
def : Device<"attiny26",     [FeatureAVR2] >; // "__AVR_ATtiny26__"
def : Device<"at90s4414",    [FeatureAVR2] >; // "__AVR_AT90S4414__"
def : Device<"at90s4433",    [FeatureAVR2] >; // "__AVR_AT90S4433__"
def : Device<"at90s4434",    [FeatureAVR2] >; // "__AVR_AT90S4434__"
def : Device<"at90s8515",    [FeatureAVR2] >; // "__AVR_AT90S8515__"
def : Device<"at90c8534",    [FeatureAVR2] >; // "__AVR_AT90C8534__"
def : Device<"at90s8535",    [FeatureAVR2] >; // "__AVR_AT90S8535__"

// Classic + MOVW, <= 8K.
def : Device<"avr25",        [FeatureAVR25] >;
def : Device<"ata6289",      [FeatureAVR25] >; // "__AVR_ATA6289__"
def : Device<"attiny13",     [FeatureAVR25] >; // "__AVR_ATtiny13__"
def : Device<"attiny13a",    [FeatureAVR25] >; // "__AVR_ATtiny13A__"
def : Device<"attiny2313",   [FeatureAVR25] >; // "__AVR_ATtiny2313__"
def : Device<"attiny2313a",  [FeatureAVR25] >; // "__AVR_ATtiny2313A__"
def : Device<"attiny24",     [FeatureAVR25] >; // "__AVR_ATtiny24__"
def : Device<"attiny24a",    [FeatureAVR25] >; // "__AVR_ATtiny24A__"
def : Device<"attiny4313",   [FeatureAVR25] >; // "__AVR_ATtiny4313__"
def : Device<"attiny44",     [FeatureAVR25] >; // "__AVR_ATtiny44__"
def : Device<"attiny44a",    [FeatureAVR25] >; // "__AVR_ATtiny44A__"
def : Device<"attiny84",     [FeatureAVR25] >; // "__AVR_ATtiny84__"
def : Device<"attiny25",     [FeatureAVR25] >; // "__AVR_ATtiny25__"
def : Device<"attiny45",     [FeatureAVR25] >; // "__AVR_ATtiny45__"
def : Device<"attiny85",     [FeatureAVR25] >; // "__AVR_ATtiny85__"
def : Device<"attiny261",    [FeatureAVR25] >; // "__AVR_ATtiny261__"
def : Device<"attiny261a",   [FeatureAVR25] >; // "__AVR_ATtiny261A__"
def : Device<"attiny461",    [FeatureAVR25] >; // "__AVR_ATtiny461__"
def : Device<"attiny461a",   [FeatureAVR25] >; // "__AVR_ATtiny461A__"
def : Device<"attiny861",    [FeatureAVR25] >; // "__AVR_ATtiny861__"
def : Device<"attiny861a",   [FeatureAVR25] >; // "__AVR_ATtiny861A__"
def : Device<"attiny43u",    [FeatureAVR25] >; // "__AVR_ATtiny43U__"
def : Device<"attiny87",     [FeatureAVR25] >; // "__AVR_ATtiny87__"
def : Device<"attiny48",     [FeatureAVR25] >; // "__AVR_ATtiny48__"
def : Device<"attiny88",     [FeatureAVR25] >; // "__AVR_ATtiny88__"
def : Device<"at86rf401",    [FeatureAVR25] >; // "__AVR_AT86RF401__"

// Classic, > 8K, <= 64K.
def : Device<"avr3",         [FeatureAVR3] >;
def : Device<"at43usb320",   [FeatureAVR3] >; // "__AVR_AT43USB320__"
def : Device<"at43usb355",   [FeatureAVR3] >; // "__AVR_AT43USB355__"
def : Device<"at76c711",     [FeatureAVR3] >; // "__AVR_AT76C711__"

// Classic, == 128K.
def : Device<"avr31",        [FeatureAVR31] >;
def : Device<"atmega103",    [FeatureAVR31] >; // "__AVR_ATmega103__"

// Classic + MOVW + JMP/CALL.
def : Device<"avr35",        [FeatureAVR35] >;
def : Device<"at90usb82",    [FeatureAVR35] >; // "__AVR_AT90USB82__"
def : Device<"at90usb162",   [FeatureAVR35] >; // "__AVR_AT90USB162__"
def : Device<"atmega8u2",    [FeatureAVR35] >; // "__AVR_ATmega8U2__"
def : Device<"atmega16u2",   [FeatureAVR35] >; // "__AVR_ATmega16U2__"
def : Device<"atmega32u2",   [FeatureAVR35] >; // "__AVR_ATmega32U2__"
def : Device<"attiny167",    [FeatureAVR35] >; // "__AVR_ATtiny167__"

// Enhanced, <= 8K.
def : Device<"avr4",         [FeatureAVR4] >;
def : Device<"atmega8",      [FeatureAVR4] >; // "__AVR_ATmega8__"
def : Device<"atmega48",     [FeatureAVR4] >; // "__AVR_ATmega48__"
def : Device<"atmega48a",    [FeatureAVR4] >; // "__AVR_ATmega48A__"
def : Device<"atmega48p",    [FeatureAVR4] >; // "__AVR_ATmega48P__"
def : Device<"atmega88",     [FeatureAVR4] >; // "__AVR_ATmega88__"
def : Device<"atmega88a",    [FeatureAVR4] >; // "__AVR_ATmega88A__"
def : Device<"atmega88p",    [FeatureAVR4] >; // "__AVR_ATmega88P__"
def : Device<"atmega88pa",   [FeatureAVR4] >; // "__AVR_ATmega88PA__"
def : Device<"atmega8515",   [FeatureAVR4] >; // "__AVR_ATmega8515__"
def : Device<"atmega8535",   [FeatureAVR4] >; // "__AVR_ATmega8535__"
def : Device<"atmega8hva",   [FeatureAVR4] >; // "__AVR_ATmega8HVA__"
def : Device<"atmega4hvd",   [FeatureAVR4] >; // "__AVR_ATmega4HVD__"
def : Device<"atmega8hvd",   [FeatureAVR4] >; // "__AVR_ATmega8HVD__"
def : Device<"at90pwm1",     [FeatureAVR4] >; // "__AVR_AT90PWM1__"
def : Device<"at90pwm2",     [FeatureAVR4] >; // "__AVR_AT90PWM2__"
def : Device<"at90pwm2b",    [FeatureAVR4] >; // "__AVR_AT90PWM2B__"
def : Device<"at90pwm3",     [FeatureAVR4] >; // "__AVR_AT90PWM3__"
def : Device<"at90pwm3b",    [FeatureAVR4] >; // "__AVR_AT90PWM3B__"
def : Device<"at90pwm81",    [FeatureAVR4] >; // "__AVR_AT90PWM81__"

// Enhanced, > 8K, <= 64K.
def : Device<"avr5",         [FeatureAVR5] >;
def : Device<"atmega16",     [FeatureAVR5] >; // "__AVR_ATmega16__"
def : Device<"atmega16a",    [FeatureAVR5] >; // "__AVR_ATmega16A__"
def : Device<"atmega161",    [FeatureAVR5] >; // "__AVR_ATmega161__"
def : Device<"atmega162",    [FeatureAVR5] >; // "__AVR_ATmega162__"
def : Device<"atmega163",    [FeatureAVR5] >; // "__AVR_ATmega163__"
def : Device<"atmega164a",   [FeatureAVR5] >; // "__AVR_ATmega164A__"
def : Device<"atmega164p",   [FeatureAVR5] >; // "__AVR_ATmega164P__"
def : Device<"atmega165",    [FeatureAVR5] >; // "__AVR_ATmega165__"
def : Device<"atmega165a",   [FeatureAVR5] >; // "__AVR_ATmega165A__"
def : Device<"atmega165p",   [FeatureAVR5] >; // "__AVR_ATmega165P__"
def : Device<"atmega168",    [FeatureAVR5] >; // "__AVR_ATmega168__"
def : Device<"atmega168a",   [FeatureAVR5] >; // "__AVR_ATmega168A__"
def : Device<"atmega168p",   [FeatureAVR5] >; // "__AVR_ATmega168P__"
def : Device<"atmega169",    [FeatureAVR5] >; // "__AVR_ATmega169__"
def : Device<"atmega169a",   [FeatureAVR5] >; // "__AVR_ATmega169A__"
def : Device<"atmega169p",   [FeatureAVR5] >; // "__AVR_ATmega169P__"
def : Device<"atmega169pa",  [FeatureAVR5] >; // "__AVR_ATmega169PA__"
def : Device<"atmega16c1",   [FeatureAVR5] >; // "__AVR_ATmega16C1__"
def : Device<"atmega16hva",  [FeatureAVR5] >; // "__AVR_ATmega16HVA__"
def : Device<"atmega16hvb",  [FeatureAVR5] >; // "__AVR_ATmega16HVB__"
def : Device<"atmega16m1",   [FeatureAVR5] >; // "__AVR_ATmega16M1__"
def : Device<"atmega16u4",   [FeatureAVR5] >; // "__AVR_ATmega16U4__"
def : Device<"atmega32",     [FeatureAVR5] >; // "__AVR_ATmega32__"
def : Device<"atmega323",    [FeatureAVR5] >; // "__AVR_ATmega323__"
def : Device<"atmega324a",   [FeatureAVR5] >; // "__AVR_ATmega324A__"
def : Device<"atmega324p",   [FeatureAVR5] >; // "__AVR_ATmega324P__"
def : Device<"atmega324pa",  [FeatureAVR5] >; // "__AVR_ATmega324PA__"
def : Device<"atmega325",    [FeatureAVR5] >; // "__AVR_ATmega325__"
def : Device<"atmega325p",   [FeatureAVR5] >; // "__AVR_ATmega325P__"
def : Device<"atmega3250",   [FeatureAVR5] >; // "__AVR_ATmega3250__"
def : Device<"atmega3250p",  [FeatureAVR5] >; // "__AVR_ATmega3250P__"
def : Device<"atmega328",    [FeatureAVR5] >; // "__AVR_ATmega328__"
def : Device<"atmega328p",   [FeatureAVR5] >; // "__AVR_ATmega328P__"
def : Device<"atmega329",    [FeatureAVR5] >; // "__AVR_ATmega329__"
def : Device<"atmega329p",   [FeatureAVR5] >; // "__AVR_ATmega329P__"
def : Device<"atmega329pa",  [FeatureAVR5] >; // "__AVR_ATmega329PA__"
def : Device<"atmega3290",   [FeatureAVR5] >; // "__AVR_ATmega3290__"
def : Device<"atmega3290p",  [FeatureAVR5] >; // "__AVR_ATmega3290P__"
def : Device<"atmega32c1",   [FeatureAVR5] >; // "__AVR_ATmega32C1__"
def : Device<"atmega32hvb",  [FeatureAVR5] >; // "__AVR_ATmega32HVB__"
def : Device<"atmega32m1",   [FeatureAVR5] >; // "__AVR_ATmega32M1__"
def : Device<"atmega32u4",   [FeatureAVR5] >; // "__AVR_ATmega32U4__"
def : Device<"atmega32u6",   [FeatureAVR5] >; // "__AVR_ATmega32U6__"
def : Device<"atmega406",    [FeatureAVR5] >; // "__AVR_ATmega406__"
def : Device<"atmega64",     [FeatureAVR5] >; // "__AVR_ATmega64__"
def : Device<"atmega640",    [FeatureAVR5] >; // "__AVR_ATmega640__"
def : Device<"atmega644",    [FeatureAVR5] >; // "__AVR_ATmega644__"
def : Device<"atmega644a",   [FeatureAVR5] >; // "__AVR_ATmega644A__"
def : Device<"atmega644p",   [FeatureAVR5] >; // "__AVR_ATmega644P__"
def : Device<"atmega644pa",  [FeatureAVR5] >; // "__AVR_ATmega644PA__"
def : Device<"atmega645",    [FeatureAVR5] >; // "__AVR_ATmega645__"
def : Device<"atmega645a",   [FeatureAVR5] >; // "__AVR_ATmega645A__"
def : Device<"atmega645p",   [FeatureAVR5] >; // "__AVR_ATmega645P__"
def : Device<"atmega6450",   [FeatureAVR5] >; // "__AVR_ATmega6450__"
def : Device<"atmega6450a",  [FeatureAVR5] >; // "__AVR_ATmega6450A__"
def : Device<"atmega6450p",  [FeatureAVR5] >; // "__AVR_ATmega6450P__"
def : Device<"atmega649",    [FeatureAVR5] >; // "__AVR_ATmega649__"
def : Device<"atmega649a",   [FeatureAVR5] >; // "__AVR_ATmega649A__"
def : Device<"atmega649p",   [FeatureAVR5] >; // "__AVR_ATmega649P__"
def : Device<"atmega6490",   [FeatureAVR5] >; // "__AVR_ATmega6490__"
def : Device<"atmega6490a",  [FeatureAVR5] >; // "__AVR_ATmega6490A__"
def : Device<"atmega6490p",  [FeatureAVR5] >; // "__AVR_ATmega6490P__"
def : Device<"atmega64c1",   [FeatureAVR5] >; // "__AVR_ATmega64C1__"
def : Device<"atmega64m1",   [FeatureAVR5] >; // "__AVR_ATmega64M1__"
def : Device<"atmega64hve",  [FeatureAVR5,
                                FeatureCoreExt] >; // "__AVR_ATmega64HVE__"
def : Device<"at90can32",    [FeatureAVR5] >; // "__AVR_AT90CAN32__"
def : Device<"at90can64",    [FeatureAVR5] >; // "__AVR_AT90CAN64__"
def : Device<"at90pwm216",   [FeatureAVR5] >; // "__AVR_AT90PWM216__"
def : Device<"at90pwm316",   [FeatureAVR5] >; // "__AVR_AT90PWM316__"
def : Device<"at90scr100",   [FeatureAVR5] >; // "__AVR_AT90SCR100__"
def : Device<"at90usb646",   [FeatureAVR5] >; // "__AVR_AT90USB646__"
def : Device<"at90usb647",   [FeatureAVR5] >; // "__AVR_AT90USB647__"
def : Device<"at94k",        [FeatureAVR5] >; // "__AVR_AT94K__"

// Enhanced, == 128K.
def : Device<"avr51",        [FeatureAVR51] >;
def : Device<"atmega128",    [FeatureAVR51] >; // "__AVR_ATmega128__"
def : Device<"atmega1280",   [FeatureAVR51] >; // "__AVR_ATmega1280__"
def : Device<"atmega1281",   [FeatureAVR51] >; // "__AVR_ATmega1281__"
def : Device<"atmega1284p",  [FeatureAVR51] >; // "__AVR_ATmega1284P__"
def : Device<"atmega128rfa1",[FeatureAVR51] >; // "__AVR_ATmega128RFA1__"
def : Device<"at90can128",   [FeatureAVR51] >; // "__AVR_AT90CAN128__"
def : Device<"at90usb1286",  [FeatureAVR51] >; // "__AVR_AT90USB1286__"
def : Device<"at90usb1287",  [FeatureAVR51] >; // "__AVR_AT90USB1287__"
def : Device<"m3000f",       [FeatureAVR51] >; // "__AVR_M3000F__"
def : Device<"m3000s",       [FeatureAVR51] >; // "__AVR_M3000S__"
def : Device<"m3001b",       [FeatureAVR51] >; // "__AVR_M3001B__"

// 3-Byte PC.
def : Device<"avr6",         [FeatureAVR6] >;
def : Device<"atmega2560",   [FeatureAVR6] >; // "__AVR_ATmega2560__"
def : Device<"atmega2561",   [FeatureAVR6] >; // "__AVR_ATmega2561__"

// Xmega, <= 8K FLASH.

// Xmega, > 8K, <= 64K FLASH, <= 64K RAM.
def : Device<"avrxmega2",    [FeatureAVRXMEGA2] >;
def : Device<"atxmega16a4",  [FeatureAVRXMEGA2] >; // "__AVR_ATxmega16A4__"
def : Device<"atxmega16d4",  [FeatureAVRXMEGA2] >; // "__AVR_ATxmega16D4__"
def : Device<"atxmega32d4",  [FeatureAVRXMEGA2] >; // "__AVR_ATxmega32D4__"

// Xmega, > 8K, <= 64K FLASH, > 64K RAM.
def : Device<"avrxmega3",    [FeatureAVRXMEGA3] >;
def : Device<"atxmega32a4",  [FeatureAVRXMEGA3] >; // "__AVR_ATxmega32A4__"

// Xmega, > 64K, <= 128K FLASH, <= 64K RAM.
def : Device<"avrxmega4",    [FeatureAVRXMEGA4] >;
def : Device<"atxmega64a3",  [FeatureAVRXMEGA4] >; // "__AVR_ATxmega64A3__"
def : Device<"atxmega64d3",  [FeatureAVRXMEGA4] >; // "__AVR_ATxmega64D3__"

// Xmega, > 64K, <= 128K FLASH, > 64K RAM.
def : Device<"avrxmega5",    [FeatureAVRXMEGA5] >;
def : Device<"atxmega64a1",  [FeatureAVRXMEGA5] >; // "__AVR_ATxmega64A1__"

// Xmega, > 128K, <= 256K FLASH, <= 64K RAM.
def : Device<"avrxmega6",    [FeatureAVRXMEGA6] >;
def : Device<"atxmega128a3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega128A3__"
def : Device<"atxmega128d3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega128D3__"
def : Device<"atxmega192a3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega192A3__"
def : Device<"atxmega192d3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega192D3__"
def : Device<"atxmega256a3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega256A3__"
def : Device<"atxmega256a3b",[FeatureAVRXMEGA6] >; // "__AVR_ATxmega256A3B__"
def : Device<"atxmega256d3", [FeatureAVRXMEGA6] >; // "__AVR_ATxmega256D3__"

// Xmega, > 128K, <= 256K FLASH, > 64K RAM.
def : Device<"avrxmega7",    [FeatureAVRXMEGA7] >;
def : Device<"atxmega128a1", [FeatureAVRXMEGA7] >; // "__AVR_ATxmega128A1__"

// Tiny
def : Device<"avrtiny1",    [FeatureAVRTINY1] >;
def : Device<"attiny4",     [FeatureAVRTINY1] >; // "__AVR_ATtiny4__"
def : Device<"attiny5",     [FeatureAVRTINY1] >; // "__AVR_ATtiny5__"
def : Device<"attiny9",     [FeatureAVRTINY1] >; // "__AVR_ATtiny9__"
def : Device<"attiny10",    [FeatureAVRTINY1] >; // "__AVR_ATtiny10__"
def : Device<"attiny20",    [FeatureAVRTINY1] >; // "__AVR_ATtiny20__"
def : Device<"attiny40",    [FeatureAVRTINY1] >; // "__AVR_ATtiny40__"



//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def AVR : Target
{
    // Pull in Instruction Info:
    let InstructionSet = AVRInstrInfo;
    let AssemblyWriters = [AVRInstPrinter];
}
