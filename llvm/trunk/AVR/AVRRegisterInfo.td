//===- AVRRegisterInfo.td - AVR Register defs ------------*- tblgen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// 
//===----------------------------------------------------------------------===//

// An AVR register prototype
// NOTE: Currently this class is designed to represent all registers available:
//       GPR, standard IO registers and extended IO registers (even if they are
//       only accessable via the Data Ram). Maybe it would be better to split 
//       it up into one class with an 6-bit Num (for GPR and std IO Reg) and a 
//       special class for the extented IO registers with an 8-bit Num. But on 
//       the other hand all registers can be accessed via the Data Ram so they 
//       belong to each other and therefor need the same Num type.
class AVRReg<string n> : Register<n>
{
  let Namespace = "AVR";
  field bits<8> Num;
}

// AVR General Purpose Register (0-32)
class GPR<bits<8> num, string n> : AVRReg<n>
{
  let Num = num;
}

// AVR Pointer Registers
class PTR<bits<8> num,string n,list<Register> subregs>: AVRReg<n>
{
  let Num = num;
  let SubRegs = subregs;
}

// AVR standard IO Register (0-64)
class IOReg<bits<8> num, string n> : AVRReg<n>
{
  let Num = num;
}

// AVR bit addressable IO Register (0-32)
class BitIOReg<bits<8> num, string n> : IOReg<num,n> {}

// AVR extended IO Registers (only accessable via Load, Store Instructions)
class ExtIOReg<bits<8> num, string n> : AVRReg<n>
{
  let Num = num;
}

// NOTE: I am not sure if it is really necessary to map the SREG to the 
//       flags but maybe this can be helpful if we add information about
//       which flags are affected by a instruction...

// AVR Flags
class AVRFlag<bits<3> num, string n> : Register<n>
{
  let Namespace = "AVR";
  field bits<3> Num = num;
}

// AVR Flag register
class FlagReg<bits<8> num, string n,list<AVRFlag> flags> : IOReg<num, n>
{
  let Num = num;
  let SubRegs = flags;
}

// AVR 8-bit Register File
def  R0 : GPR< 0,  "r0">, DwarfRegNum<[ 0]>;
def  R1 : GPR< 1,  "r1">, DwarfRegNum<[ 1]>;
def  R2 : GPR< 2,  "r2">, DwarfRegNum<[ 2]>;
def  R3 : GPR< 3,  "r3">, DwarfRegNum<[ 3]>;
def  R4 : GPR< 4,  "r4">, DwarfRegNum<[ 4]>;
def  R5 : GPR< 5,  "r5">, DwarfRegNum<[ 5]>;
def  R6 : GPR< 6,  "r6">, DwarfRegNum<[ 6]>;
def  R7 : GPR< 7,  "r7">, DwarfRegNum<[ 7]>;
def  R8 : GPR< 8,  "r8">, DwarfRegNum<[ 8]>;
def  R9 : GPR< 9,  "r9">, DwarfRegNum<[ 9]>;
def R10 : GPR<10, "r10">, DwarfRegNum<[10]>;
def R11 : GPR<11, "r11">, DwarfRegNum<[11]>;
def R12 : GPR<12, "r12">, DwarfRegNum<[12]>;
def R13 : GPR<13, "r13">, DwarfRegNum<[13]>;
def R14 : GPR<14, "r14">, DwarfRegNum<[14]>;
def R15 : GPR<15, "r15">, DwarfRegNum<[15]>;
def R16 : GPR<16, "r16">, DwarfRegNum<[16]>;
def R17 : GPR<17, "r17">, DwarfRegNum<[17]>;
def R18 : GPR<18, "r18">, DwarfRegNum<[18]>;
def R19 : GPR<19, "r19">, DwarfRegNum<[19]>;
def R20 : GPR<20, "r20">, DwarfRegNum<[20]>;
def R21 : GPR<21, "r21">, DwarfRegNum<[21]>;
def R22 : GPR<22, "r22">, DwarfRegNum<[22]>;
def R23 : GPR<23, "r23">, DwarfRegNum<[23]>;
def R24 : GPR<24, "r24">, DwarfRegNum<[24]>;
def R25 : GPR<25, "r25">, DwarfRegNum<[25]>;
def R26 : GPR<26, "r26">, DwarfRegNum<[26]>;
def R27 : GPR<27, "r27">, DwarfRegNum<[27]>;
def R28 : GPR<28, "r28">, DwarfRegNum<[28]>;
def R29 : GPR<29, "r29">, DwarfRegNum<[29]>;
def R30 : GPR<30, "r30">, DwarfRegNum<[30]>;
def R31 : GPR<31, "r31">, DwarfRegNum<[31]>;

let Namespace = "AVR" in {
// Note: Code depends on these having consecutive numbers.
def ssub_0  : SubRegIndex;
def ssub_1  : SubRegIndex;
def ssub_2  : SubRegIndex;
def ssub_3  : SubRegIndex;
def ssub_4  : SubRegIndex;
def ssub_5  : SubRegIndex;
def ssub_6  : SubRegIndex;
def ssub_7  : SubRegIndex;
}

let SubRegIndices = [ssub_0,ssub_1] in {
// AVR 16-bit Address Pointer Register
def X_PTR : PTR<26,"X", [R26, R27]>, DwarfRegNum<[26]>;
def Y_PTR : PTR<28,"Y", [R28, R29]>, DwarfRegNum<[28]>;
def Z_PTR : PTR<30,"Z", [R30, R31]>, DwarfRegNum<[30]>;

// AVR Word Registers
def   R1R0 : PTR< 0,  "r1:0" , [ R0,  R1]>, DwarfRegNum<[ 0]>;
def   R3R2 : PTR< 2,  "r3:2" , [ R2,  R3]>, DwarfRegNum<[ 2]>;
def   R5R4 : PTR< 4,  "r5:4" , [ R4,  R5]>, DwarfRegNum<[ 4]>;
def   R7R6 : PTR< 6,  "r7:6" , [ R6,  R7]>, DwarfRegNum<[ 6]>;
def   R9R8 : PTR< 8,  "r9:8" , [ R8,  R9]>, DwarfRegNum<[ 8]>;
def R11R10 : PTR<10, "r11:10", [R10, R11]>, DwarfRegNum<[10]>;
def R13R12 : PTR<12, "r13:12", [R12, R13]>, DwarfRegNum<[12]>;
def R15R14 : PTR<14, "r15:14", [R14, R15]>, DwarfRegNum<[14]>;
def R17R16 : PTR<16, "r17:16", [R16, R17]>, DwarfRegNum<[16]>;
def R19R18 : PTR<18, "r19:18", [R18, R19]>, DwarfRegNum<[18]>;
def R21R20 : PTR<20, "r21:20", [R20, R21]>, DwarfRegNum<[20]>;
def R23R22 : PTR<22, "r23:22", [R22, R23]>, DwarfRegNum<[22]>;
def R25R24 : PTR<24, "r25:24", [R24, R25]>, DwarfRegNum<[24]>;
def R27R26 : PTR<26, "r27:26", [R26, R27]>, DwarfRegNum<[26]>;
def R29R28 : PTR<28, "r29:28", [R28, R29]>, DwarfRegNum<[28]>;
def R31R30 : PTR<30, "r31:30", [R30, R31]>, DwarfRegNum<[30]>;
}

let SubRegIndices = [ssub_0,ssub_1,ssub_2,ssub_3] in {
// AVR Double Word Registers
def R25R22 : PTR<22, "r25-r22", [R22,R23,R24,R25]>, DwarfRegNum<[22]>;
def R21R18 : PTR<18, "r21-r18", [R18,R19,R20,R21]>, DwarfRegNum<[18]>;
def R17R14 : PTR<14, "r17-r14", [R14,R15,R16,R17]>, DwarfRegNum<[14]>;
def R13R10 : PTR<10, "r13-r10", [R10,R11,R12,R13]>, DwarfRegNum<[10]>;
}

let SubRegIndices = [ssub_0,ssub_1,ssub_2,ssub_3,ssub_4,ssub_5,ssub_6,ssub_7] in {
// AVR Quad Word Registers
def R25R18 : PTR<18, "r25-r18", [R18,R19,R20,R21,R22,R23,R24,R25]>, DwarfRegNum<[18]>;
def R17R10 : PTR<10, "r17-r10", [R10,R11,R12,R13,R14,R15,R16,R17]>, DwarfRegNum<[10]>;
}

// AVR status flags
def C : AVRFlag<0,"C">;
def Z : AVRFlag<1,"Z">;
def N : AVRFlag<2,"N">; 
def V : AVRFlag<3,"V">;
def S : AVRFlag<4,"S">;
def H : AVRFlag<5,"H">;
def T : AVRFlag<6,"T">;
def I : AVRFlag<7,"I">;

// AVR status register
//TODO: swap dir??
let SubRegIndices = [ssub_0,ssub_1,ssub_2,ssub_3,ssub_4,ssub_5,ssub_6,ssub_7] in {
def SREG : FlagReg<0x5F,"SREG", [C,Z,N,V,S,H,T,I]>, DwarfRegNum<[0x5F]>;
}

// AVR stack registers
def SPH : IOReg<0x5E,"SPH">, DwarfRegNum<[0x5E]>;
def SPL : IOReg<0x5D,"SPL">, DwarfRegNum<[0x5D]>;

// AVR stack pointer
let SubRegIndices = [ssub_0,ssub_1] in {
def SP : PTR<0x5D,"SP", [SPL, SPH]>, DwarfRegNum<[0x5D]>;
}

// Gereral Purpose Regsister Class
// NOTE: If an allocation_order_* methode is not specified, than regList also 
//       defines the order of allocation used by the register allocator. 
def GPRegs : RegisterClass<"AVR", [i8], 8, [ 
                                // Call-used registers (Caller saved)
                                           R24, R25,
                                           R18, R19,
                                           R20, R21,
                                           R22, R23,
                                           R30, R31,
                                           R26, R27,
			                             // Call-saved registers (Callee saved)
                                      R28, R29, R17, R16, R15, R14,
					                                 R13, R12, R11, R10, R9, R8,
					                                 R7, R6, R5, R4, R3, R2, 
			                             // Fixed registers
					                                       R1, R0]> 
{
    // NOTE: The allocation_order_end methode is implemented to tell the 
    //       register allocator not to use fixed registers!
  let MethodProtos = 
  [{
      iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  let MethodBodies = 
  [{
      GPRegsClass::iterator
      GPRegsClass::allocation_order_end(const MachineFunction &MF) const
      {
        return end()-2;	// R1 and R0 are reseverd for fixed purposes
      }
  }];
}

// GPRs to use with the ADIW instruction
def ADIWRegs : RegisterClass<"AVR", [i16], 16, [
                              // Call-used registers (Caller saved)
                                           R25R24,
                                           R31R30,
                                           R27R26,
			                           // Call-saved registers (Callee saved)
					                                       R29R28]> {}

def LDIRegs : RegisterClass<"AVR", [i8], 8, [
                              // Call-used registers (Caller saved)
                                           R24, R25, 
                                           R18, R19, 
                                           R20, R21, 
                                           R22, R23,
                                           R30, R31, 
                                           R26, R27,
			                           // Call-saved registers (Callee saved)
					                                       R28, R29,
					                                       R17, R16]> {}
					                                       
//def NoLDIGPRegs : RegisterClass<"AVR", [i8], 8, [
//			                            // Call-saved registers (Callee saved)
//					                           R15, R14, R13, R12, R11, R10, R9, 
//					                           R8, R7, R6, R5, R4, R3, R2, 
//			                            // Fixed registers
//					                           R1, R0]>
//{
//  // NOTE: The allocation_order_end methode is implemented to tell the 
//  //       register allocator not to use fixed registers!
//  let MethodProtos = 
//  [{
//      iterator allocation_order_end(const MachineFunction &MF) const;
//  }];
//  let MethodBodies = 
//  [{
//      NoLDIGPRegsClass::iterator
//      NoLDIGPRegsClass::allocation_order_end(const MachineFunction &MF) const
//      {
//        return end()-2;	// R1 and R0 are reseverd for fixed purposes
//      }
//  }];
//}

// registers to use with the MULSU and the FMULxx instructions
//def FMULRegs : RegisterClass<"AVR", [i8], 8, [
//                              // Call-used registers (Caller saved)
//                                           R18, R19, 
//                                           R20, R21, 
//                                           R22, R23,
//			                           // Call-saved registers (Callee saved)
//					                                       R17, R16]> {}
 
// WordRegister groups to use with the movw instruction
def WRegs : RegisterClass<"AVR", [i16], 16, [
                              // Call-used registers (Caller saved)
                                           R25R24, 
                                           R19R18, 
                                           R21R20, 
                                           R23R22,
                                           R31R30, 
                                           R27R26,
			                           // Call-saved registers (Callee saved)
					                                     R29R28,
					                                     R17R16,
					                                     R15R14,
					                                     R13R12,
					                                     R11R10,
					                                     R9R8,
					                                     R7R6,
					                                     R5R4,
					                                     R3R2,
			                           // Fixed registers
					                                       R1R0]> 
{
  // NOTE: The allocation_order_end methode is implemented to tell the 
  //       register allocator not to use fixed registers!
  let MethodProtos = 
  [{
      iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  
  let MethodBodies = 
  [{
      WRegsClass::iterator
      WRegsClass::allocation_order_end(const MachineFunction &MF) const
      {
        return end()-1;	// R1R0 are reseverd for fixed purposes
      }
  }];
}

// Double Word Registers
def DWRegs : RegisterClass<"AVR", [i32], 32, [R25R22,
                                              R21R18,
                                              R17R14,
                                              R13R10]> {}

// Quad Word Registers
def QWRegs : RegisterClass<"AVR", [i64], 64, [R25R18, R17R10]> {}

// Pointer Registers
def PTRRegs : RegisterClass<"AVR", [i16], 16, [X_PTR, Y_PTR, Z_PTR]> {}

// Base Pointer Registers
def BasePTRRegs : RegisterClass<"AVR", [i16], 16, [Y_PTR, Z_PTR]> {}                             

// Mul return register R1:R0
def MulRetReg : RegisterClass<"AVR", [i16], 16, [R1R0]> {}
                                              
// AVR status flags
def StatusFlags : RegisterClass<"AVR", [i1], 1, [C,Z,N,V,S,H,T,I]> {}
