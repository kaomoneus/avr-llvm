//===------- AVRRegisterInfo.td - AVR Register defs -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the AVR register file
//===----------------------------------------------------------------------===//

class AVRReg<bits<5> num, string n, list<Register> subregs = []>
             : RegisterWithSubRegs<n, subregs>
{
  field bits<5> Num = num;
  let Namespace = "AVR";
  let Aliases = subregs;
}

/*
class AVRStatFlags<bits<3> b, string n> : AVRReg<n>
{
  field bits<3> Num = b;
}*/

//===----------------------------------------------------------------------===//
//  Register definitions
//===----------------------------------------------------------------------===//

//:TODO: OK here is an important file, i've left all register pair classes,
// in case we find a way way for using them, for now we'll stick with 8bit regs
// remove this once we decide what to do.

// 1 byte register group
def R0 : AVRReg<0, "r0">, DwarfRegNum<[0]>;
def R1 : AVRReg<1, "r1">, DwarfRegNum<[1]>;
def R2 : AVRReg<2, "r2">, DwarfRegNum<[2]>;
def R3 : AVRReg<3, "r3">, DwarfRegNum<[3]>;
def R4 : AVRReg<4, "r4">, DwarfRegNum<[4]>;
def R5 : AVRReg<5, "r5">, DwarfRegNum<[5]>;
def R6 : AVRReg<6, "r6">, DwarfRegNum<[6]>;
def R7 : AVRReg<7, "r7">, DwarfRegNum<[7]>;
def R8 : AVRReg<8, "r8">, DwarfRegNum<[8]>;
def R9 : AVRReg<9, "r9">, DwarfRegNum<[9]>;
def R10 : AVRReg<10, "r10">, DwarfRegNum<[10]>;
def R11 : AVRReg<11, "r11">, DwarfRegNum<[11]>;
def R12 : AVRReg<12, "r12">, DwarfRegNum<[12]>;
def R13 : AVRReg<13, "r13">, DwarfRegNum<[13]>;
def R14 : AVRReg<14, "r14">, DwarfRegNum<[14]>;
def R15 : AVRReg<15 ,"r15">, DwarfRegNum<[15]>;
def R16 : AVRReg<16, "r16">, DwarfRegNum<[16]>;
def R17 : AVRReg<17, "r17">, DwarfRegNum<[17]>;
def R18 : AVRReg<18, "r18">, DwarfRegNum<[18]>;
def R19 : AVRReg<19, "r19">, DwarfRegNum<[19]>;
def R20 : AVRReg<20, "r20">, DwarfRegNum<[20]>;
def R21 : AVRReg<21, "r21">, DwarfRegNum<[21]>;
def R22 : AVRReg<22, "r22">, DwarfRegNum<[22]>;
def R23 : AVRReg<23, "r23">, DwarfRegNum<[23]>;
def R24 : AVRReg<24, "r24">, DwarfRegNum<[24]>;
def R25 : AVRReg<25, "r25">, DwarfRegNum<[25]>;
def R26 : AVRReg<26, "r26">, DwarfRegNum<[26]>;
def R27 : AVRReg<27, "r27">, DwarfRegNum<[27]>;
def R28 : AVRReg<28, "r28">, DwarfRegNum<[28]>;
def R29 : AVRReg<29, "r29">, DwarfRegNum<[29]>;
def R30 : AVRReg<30, "r30">, DwarfRegNum<[30]>;
def R31 : AVRReg<31, "r31">, DwarfRegNum<[31]>;
/*
// 2 byte group registers
def R31R30 : AVRReg<30, "r30", [R30, R31]>, DwarfRegNum<[30]>;
def R29R28 : AVRReg<28, "r28", [R28, R29]>, DwarfRegNum<[28]>;
def R27R26 : AVRReg<26, "r26", [R26, R27]>, DwarfRegNum<[26]>;
def R25R24 : AVRReg<24, "r24", [R24, R25]>, DwarfRegNum<[24]>;
def R23R22 : AVRReg<22, "r22", [R22, R23]>, DwarfRegNum<[22]>;
def R21R20 : AVRReg<20, "r20", [R20, R21]>, DwarfRegNum<[20]>;
def R19R18 : AVRReg<18, "r18", [R18, R19]>, DwarfRegNum<[18]>;
def R17R16 : AVRReg<16, "r16", [R16, R17]>, DwarfRegNum<[16]>;
def R15R14 : AVRReg<14, "r14", [R14, R15]>, DwarfRegNum<[14]>;
def R13R12 : AVRReg<12, "r12", [R12, R13]>, DwarfRegNum<[12]>;
def R11R10 : AVRReg<10, "r10", [R10, R11]>, DwarfRegNum<[10]>;
def R9R8 : AVRReg<8, "r8", [R8, R9]>, DwarfRegNum<[8]>;
def R7R6 : AVRReg<6, "r6", [R6, R7]>, DwarfRegNum<[6]>;
def R5R4 : AVRReg<4, "r4", [R4, R5]>, DwarfRegNum<[4]>;
def R3R2 : AVRReg<2, "r2", [R2, R3]>, DwarfRegNum<[2]>;
def R1R0 : AVRReg<0, "r0", [R0, R1]>, DwarfRegNum<[0]>;
*/
// 4 byte group registers
/*
def R25R22 : AVRReg<"r22", [R23R22, R25R24]>, DwarfRegNum<[22]>;
def R21R18 : AVRReg<"r18", [R19R18, R21R20]>, DwarfRegNum<[18]>;
def R17R14 : AVRReg<"r14", [R15R14, R17R16]>, DwarfRegNum<[14]>;
def R13R10 : AVRReg<"r10", [R11R10, R13R12]>, DwarfRegNum<[10]>;

// 8 byte group registers
def R25R18 : AVRReg<"r18", [R21R18, R25R22]>, DwarfRegNum<[18]>;
def R17R10 : AVRReg<"r10", [R13R10, R17R14]>, DwarfRegNum<[10]>;
*/
// AVR status flags
/*
def C : AVRStatFlags<0, "C">;       // Carry flag
def Z : AVRStatFlags<1, "Z">;       // Zero flag
def N : AVRStatFlags<2, "N">;       // Negative flag
def V : AVRStatFlags<3, "V">;       // Two's complement overflow indicator
def S : AVRStatFlags<4, "S">;       // N xor V, for signed tests
def H : AVRStatFlags<5, "H">;       // Half carry flag
def T : AVRStatFlags<6, "T">;       // Transfer bit used by BLT and BST
def I : AVRStatFlags<7, "I">;       // Global interrupt enable/disable flag
*/
//:TODO: this is a test, see if we need them for EXTRACT_SUBREG!
/*
def AVR_subreg_8bit    : PatLeaf<(i32 1)>;
def AVR_subreg_8bit_hi : PatLeaf<(i32 2)>;

def : SubRegSet<1, [R31R30, R29R28, R27R26, R25R24, R23R22, R21R20, R19R18,
                    R17R16, R15R14, R13R12, R11R10, R9R8, R7R6, R5R4, R3R2,
                    R1R0],
                   [R31, R29, R27, R25, R23, R21, R19, R17,
                    R15, R13, R11, R9, R7, R5, R3, R1]>;

def : SubRegSet<2, [R31R30, R29R28, R27R26, R25R24, R23R22, R21R20, R19R18,
                    R17R16, R15R14, R13R12, R11R10, R9R8, R7R6, R5R4, R3R2,
                    R1R0],
                   [R30, R28, R26, R24, R22, R20, R18, R16,
                    R14, R12, R10, R8, R6, R4, R2, R0]>;*/

//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//
//:TODO: R0 and R1 should be skipped by the custom iterators
def GPR8 : RegisterClass<"AVR", [i8], 8,
                         (add
                          // return value and arguments
                          R24, R25, R18, R19, R20, R21, R22, R23,
                          // scratch registers
                          R30, R31, R26, R27,
                          // callee saved registers
                          R28, R29, R17, R16, R15, R14, R13, R12, R11, R10,
                          R9, R8, R7, R6, R5, R4, R3, R2, R0, R1
                         )>;

def GPR8_E : RegisterClass<"AVR", [i8], 8,
                           (add
                            // return value and arguments
                            R24, R18, R20, R22,
                            // scratch registers
                            R30, R26,
                            // callee saved registers
                            R28, R16, R14, R12, R10,
                            R8, R6, R4, R2, R0
                           )>
{
  //let SubRegClassList = [GPR8];
}

def GPR8_O : RegisterClass<"AVR", [i8], 8,
                           (add
                            // return value and arguments
                            R25, R19, R21, R22,
                            // scratch registers
                            R30, R26,
                            // callee saved registers
                            R28, R16, R14, R12, R10,
                            R8, R6, R4, R2, R1
                           )>
{
  //let SubRegClassList = [GPR8];
}

/*
def WDREGS : RegisterClass<"AVR", [v2i8, i16], 16,
                           (add
                            // return value and arguments
                            R25R24, R19R18, R21R20, R23R22,
                            // scratch registers
                            R31R30, R27R26,
                            // callee saved registers
                            R29R28, R17R16, R15R14, R13R12, R11R10,
                            R9R8, R7R6, R5R4, R3R2, R1R0
                           )>
{
  //let SubRegClassList = [GPR8, GPR8];
}
*/
//def DWREGS : RegisterClass<"AVR", [i32], 32, [R25R22, R21R18, R17R14, R13R10]>;

//def QWREGS : RegisterClass<"AVR", [i64], 64, [R25R18, R17R10]>;

def LDREGS : RegisterClass<"AVR", [i8], 8,
                           (add
                            // return value and arguments
                            R24, R25, R18, R19, R20, R21, R22, R23,
                            // scratch registers
                            R30, R31, R26, R27,
                            // callee saved registers
                            R28, R29, R17
                           )>;

/*
def SREG : RegisterClass<"AVR", [i1], 1, [C, Z, N, V, S, H, T, I]>
{
    let CopyCost = -1;      // Don't allow copying of status registers
}
*/
//:TODO: this is just a hack, remove it once add16 works!
def SREG : AVRReg<14, "FLAGS">, DwarfRegNum<[88]>;
def CCR : RegisterClass<"AVR", [i8], 8, (add SREG)>
{
  let CopyCost = -1;      // Don't allow copying of status registers
}
