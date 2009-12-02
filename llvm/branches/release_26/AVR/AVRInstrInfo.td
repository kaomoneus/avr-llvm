//===- AVRInstrInfo.td - AVR Register defs -------------------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "AVRInstrFormats.td"
//===----------------------------------------------------------------------===//
// Feature predicates.
//===----------------------------------------------------------------------===//

def HasJMP   : Predicate<"Subtarget.hasJMP()">;
def HasMUL   : Predicate<"Subtarget.hasMUL()">;
def HasMOVW  : Predicate<"Subtarget.hasMOVW()">;
def HasJTAG  : Predicate<"Subtarget.hasJTAG()">;
def HasEIJMP : Predicate<"Subtarget.hasEIJMP()">;
def HasSPM   : Predicate<"Subtarget.hasSPM()">;
def HasDES   : Predicate<"Subtarget.hasDES()">;

def IsAVR2   : Predicate<"Subtarget.isAVR2()">;
def IsAVR25  : Predicate<"Subtarget.isAVR25()">;
def IsAVR3   : Predicate<"Subtarget.isAVR3()">;
def IsAVR31  : Predicate<"Subtarget.isAVR31()">;
def IsAVR4   : Predicate<"Subtarget.isAVR4()">;
def IsAVR5   : Predicate<"Subtarget.isAVR5()">;
def IsAVR51  : Predicate<"Subtarget.isAVR51()">;
def IsAVR6   : Predicate<"Subtarget.isAVR6()">;
def IsXMEGA  : Predicate<"Subtarget.isXMEGA()">;

//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

// FIXME: Need to understand the following defs. Now just copy paste from
//        the X86 and PIC16 Targets...

// These are target-independent nodes, but have target-specific formats.
def SDT_AVRCallSeqStart : SDTypeProfile<0, 1, [ SDTCisVT<0, i8> ]>;
def SDT_AVRCallSeqEnd : SDTypeProfile<0, 1, [ SDTCisVT<0, i8> ]>;

def AVRcallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_AVRCallSeqStart,
                           [SDNPHasChain, SDNPOutFlag]>;
def AVRcallseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_AVRCallSeqEnd,
                           [SDNPHasChain, SDNPOutFlag]>;

let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : Pseudo<255, (outs), (ins i8imm:$amt),
                               "!ADJCALLSTACKDOWN $amt",
                               [(AVRcallseq_start imm:$amt)]>;
def ADJCALLSTACKUP : Pseudo<254, (outs), (ins i8imm:$amt),
                            "!ADJCALLSTACKUP $amt",
                            [(AVRcallseq_end imm:$amt)]>;
}

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

/*
multiclass IndirDataAccess<string OpcStr,bits<1> dir, bits<1> reg> {
  def unc : 
  def pi  :
  def pd  :
  def dis : 
*/

//===----------------------------------------------------------------------===//
// AVR Condition Codes
//===----------------------------------------------------------------------===//
// test on SREG flags
def AVR_COND_C  : PatLeaf<(i8 0)>; // Carry Flag
def AVR_COND_Z  : PatLeaf<(i8 1)>; // Zero Flag
def AVR_COND_N  : PatLeaf<(i8 2)>; // Negative Flag
def AVR_COND_V  : PatLeaf<(i8 3)>; // Two's complement overflow indicator
def AVR_COND_S  : PatLeaf<(i8 4)>; // Sign Test
def AVR_COND_H  : PatLeaf<(i8 5)>; // Half Carry Flag
def AVR_COND_T  : PatLeaf<(i8 6)>; // Transfer bit
def AVR_COND_I  : PatLeaf<(i8 7)>; // Global Interupt
/*
// conditional tests
// bit is set
def AVR_COND_E  : PatLeaf<(i8 1)>; // Equal
def AVR_COND_LO : PatLeaf<(i8 0)>; // Lower (unsigned)
def AVR_COND_LT : PatLeaf<(i8 4)>; // Less Then (signed)
def AVR_COND_MI : PatLeaf<(i8 2)>; // Minus
// bit is cleared
def AVR_COND_GE : PatLeaf<(i8 4)>; // Greater or Equal (signed)
def AVR_COND_NE : PatLeaf<(i8 1)>; // Not Equal
def AVR_COND_PL : PatLeaf<(i8 2)>; // Plus
def AVR_COND_SH : PatLeaf<(i8 0)>; // Same or higher (unsigned)
*/

//===----------------------------------------------------------------------===//
// AVR addressing mode. 
//===----------------------------------------------------------------------===//
// It matches address of globals as well as the stack slots
// that are created for locals and temporaries. This addressing mode
// converts the GlobalAddress and FrameIndex nodes to TargetGlobalAddress
// and TargetFrameIndex nodes.

/*

def diraddrmode : ComplexPattern<i16, 2, "SelectDirectAM", [frameindex], []>;
def dirloadmode : ComplexPattern<i16, 2, "LoadNothing", [frameindex], []>;
def indirloadmode : ComplexPattern<i16, 2, "LoadFSR", [frameindex], []>;


// Address operand.
def mem : Operand<i16> {
  let PrintMethod = "printAddrModeOperand";
  let MIOperandInfo = (ops i16imm, PTRRegs);
}

// Instruction operand types
def simm8      : Operand<i8>;

def AVRWrapper  : SDNode<"AVRISD::Wrapper", SDTIntUnaryOp>;

// so_imm_XFORM - Return a so_imm value packed into the format described for
// so_imm def below.
def so_imm_XFORM : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((int8_t)N->getZExtValue(), MVT::i32);
}]>;

def so_imm : Operand<i8>,
             PatLeaf<(imm), [{}]> {
  let PrintMethod = "printSOImmOperand";
}



// AVR Address Mode! SDNode frameindex could possibily be a match 
// since load and store instructions from stack used it.
def addr : Operand<i16>;

*/

def i16immSExt8  : PatLeaf<(i16 imm), [{
  // i16immSExt8 predicate - True if the 16-bit immediate fits in a 8-bit
  // sign extended field.
  return (int16_t)N->getZExtValue() == (int8_t)N->getZExtValue();
}]>;


// 16-bits but only 8 bits are significant.
def i16i8imm  : Operand<i16>;

// most of the avr instructions are 2-addr. There is also
// a isTwoAddress flag in the Instruction class but
// Constraints are more flexible and should be used for
// new implementations. 
let Constraints = "$src1 = $dst" in {
// Arthmetic or Logic operation with one register 
class ArithR<bits<11> op, string instr_asm, list<dag> pattern>:
  DirAddrSingleReg< op,
      (outs GPRegs:$dst),
      (ins GPRegs:$src1),
      !strconcat(instr_asm, " $src1"),
      pattern>;

// Arithmetic or Logic operation with two registers
class ArithRR<bits<6> op, string instr_asm, SDNode OpNode>:
  DirAddrTwoReg< op,
      (outs GPRegs:$dst),
      (ins GPRegs:$src1, GPRegs:$src2),
      !strconcat(instr_asm, " $src1, $src2"),
      [(set GPRegs:$dst, (OpNode GPRegs:$src1, GPRegs:$src2))]>;

// Arithmetic or Logic operation with one register and one immediate 8-bit
class ArithRI8<bits<4> op, string instr_asm, SDNode OpNode>:
  OpImm< op,
      (outs GPRegs:$dst),
      (ins GPRegs:$src1, i8imm:$src2),
      !strconcat(instr_asm, " $src1, $src2"),
      [(set GPRegs:$dst, (OpNode GPRegs:$src1, imm:$src2))]>;
} // Constraints = "$src1 = $dst" 

//===----------------------------------------------------------------------===//
// Instruction definition
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AVR Instructions
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Arithmetic and logic instructions
//===----------------------------------------------------------------------===//

// Add two Registers
def ADD  : ArithRR<0b000011,"add",add>;
// Add with Carry
let Uses =[C] in {
  def ADC  : ArithRR< 0b000111,"adc", addc>;
/*  def ADC  : DirAddrTwoReg< 0b000111,
      (outs GPRegs:$dst),
      (ins GPRegs:$src1, GPRegs:$src2),
      "adc $src1, $src2",
      []>;*/
}
// Add Intermediate to Word
let  Constraints = "$src1 = $dst" in {
  def ADIW : WOpImm<0b10010110,(outs ADIWRegs:$dst),
             (ins ADIWRegs:$src1, i16i8imm:$src2),
  	     "adiw $src1, $src2",
  	     [(set ADIWRegs:$dst, (add ADIWRegs:$src1, i16immSExt8:$src2))]>;
}
// Subtract without Carry
def SUB  : ArithRR<0b000110,"sub",sub>;
// Subtract Immediate
def SUBI : ArithRI8<0b0101,"subi",sub>;

let Uses = [C] in {
 // Substract with Carry
 def SBC  : ArithRR< 0b000010,"sbc",subc>;
/* def SBC  : DirAddrTwoReg< 0b000010,
      (outs GPRegs:$dst),
      (ins GPRegs:$src1, GPRegs:$src2),
      "sbc $src1, $src2",
      []>;*/
 // Substract Immediate with Carry
 def SBCI : ArithRI8<0b0100,"sbci",sub>;
}
// Substract Immediate from Word
let Constraints = "$src1 = $dst" in {
  def SBIW : WOpImm<0b10010111,(outs ADIWRegs:$dst),
             (ins ADIWRegs:$src1, i16i8imm:$src2),
             "sbiw $src1, $src2",
             [(set ADIWRegs:$dst, (sub ADIWRegs:$src1, i16immSExt8:$src2))]>;
}
// Logical AND
def AND : ArithRR<0b001000,"and",and>;
// Logical AND with Immediate
def ANDI : ArithRI8<0b0111, "andi", and>;
// Logical OR
def OR : ArithRR<0b001010, "or", or>;
// Logical AND with Immediate
def ORI : ArithRI8<0b0110, "or", or>;
// Exclusive OR
def EOR : ArithRR<0b001001, "eor", xor>;
// One's Complement
def COM : ArithR<0b10010100000, "com",
	  [(set GPRegs:$dst, (sub 0xff, GPRegs:$src1))]>;
// Two's Complement
def NEG : ArithR<0b10010100001, "neg",
          [(set GPRegs:$dst, (sub 0, GPRegs:$src1))]>;
// Set Bit(s) in Register FIXME: Same as ORI
def SBR : ArithRI8<0b0110, "sbr", or>;
// Clear Bit(s) in Register NOTE: Same as ANDI with the complement of K
let Constraints = "$src1 = $dst" in {
def CBR : OpImm<0b0111,
          (outs GPRegs:$dst),
          (ins GPRegs:$src1, i8imm:$src2),
          "cbr $src1, $src2",
          [(set GPRegs:$dst, (and GPRegs:$src1,(sub 0xFF, imm:$src2)))]>;
}
// Increment
def INC : ArithR<0b1001010001, "inc", 
          [(set GPRegs:$dst , (and GPRegs:$src1, 1))]>;
// Decrement
def DEC : ArithR<0b1001010101, "dec", 
          [(set GPRegs:$dst , (sub GPRegs:$src1, 1))]>;
// FIXME: Need to find a way to set Rr = Rd??
/*
// Test for Zero or Minus FIXME: Same as AND Rd,Rd
def TST : DirAddrTwoReg<0b001000,(outs GPRegs:$dst),(ins GPRegs:$dst),
          "tst $dst",
	  []>;
// Clear Register FIXME: Same as EOR Rd, Rd
def CLR : DirAddrTwoReg<0b001001, (outs GPRegs:$dst),(ins ),
          "clr $dst",
	  [(set GPRegs:$dst, 0x00)]>;
}*/
// Set Register
// NOTE: the SER instruction only takes only 4 bit register descriptor
//       but the MSB of the LDI registers is alway 1 so no special format
//       must be used
let Rd = 0b11111 in {
def SER : DirAddrTwoReg<0b111011, (outs LDIRegs:$dst),(ins ),
          "clr $dst",
	  [(set LDIRegs:$dst, 0xff)]>;
}

/*
FIXME: need to finde a way to zero/sign extend the source register
from i8 to i16
let isCommutable = 1 in {
// Multiply Unsigned
def MUL : DirAddrTwoReg<0b100111,(outs MulRetReg:$dst),
           (ins GPRegs:$src1, GPRegs:$src2),
	   "mul $src1, $src2",
	   [(set MulRetReg:$dst, (mul GPRegs:$src1, GPRegs:$src2))]>;
// Multiply Signed
def MULS : DirAddrTwoWReg<0b0000010,(outs MulRetReg:$dst),
           (ins LDIRegs:$src1, LDIRegs:$src2),
	   "muls $src1, $src2",
	   [(set MulRetReg:$dst, (mul LDIRegs:$src1, LDIRegs:$src2))]>;
} // isCommutable

// Multiply signed with unsigned
def MULSU : DirAddrTwoFmulReg<0b000001100,(outs MulRetReg:$dst),
            (ins FMULRegs:$src1, FMULRegs:$src2),
	    "mulsu $src1, $src2",
	    [(set MulRetReg:$dst, (mul FMULRegs:$src1, FMULRegs:$src2))]>;

let isCommutable = 1 in {
// Fractional Multiply Unsigned
def FMUL : DirAddrTwoFmulReg<0b0000001101,(outs MulRetReg:$dst),
            (ins FMULRegs:$src1, FMULRegs:$src2),
	    "fmul $src1, $src2",
	    [(set MulRetReg:$dst, (mul FMULRegs:$src1, FMULRegs:$src2))]>;
// Fractional Multiply Signed
def FMULS : DirAddrTwoFmulReg<0b0000001110,(outs MulRetReg:$dst),
            (ins FMULRegs:$src1, FMULRegs:$src2),
	    "fmuls $src1, $src2",
	    [(set MulRetReg:$dst, (mul FMULRegs:$src1, FMULRegs:$src2))]>;
} // is Commutable

// Fractional Multiply Signed with Unsigned
def FMULSU : DirAddrTwoFmulReg<0b0000001111,(outs MulRetReg:$dst),
            (ins FMULRegs:$src1, FMULRegs:$src2),
	    "fmulsu $src1, $src2",
	    [(set MulRetReg:$dst, (mul FMULRegs:$src1, FMULRegs:$src2))]>;

*/
// TODO: DES Data Encryption

//===----------------------------------------------------------------------===//
// Branch instructions
//===----------------------------------------------------------------------===//

def brtarget : Operand<OtherVT>;
def calltarget : Operand<i32>;

let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
// Relative Jump
  def RJMP : RelProgAddr<0b1100,(outs),(ins brtarget:$dst),
           "rjmp $dst",
           [(br bb:$dst)]>;
// Jump
  def JMP : ProgDirAddr<0b1001010110,(outs),(ins brtarget:$dst),
          "jmp $dst",
          [(br bb:$dst)]>;
}

let Uses = [Z], isIndirectBranch = 1, Instr = 0b1001010000001001,
    isBranch = 1, isTerminator = 1, isBarrier = 1 in {
// Indirect Jump
  def IJMP : WordInst<(outs),(ins),
            "ijmp",
           []>; 
// TODO: Extended Indirect Jump
}
/*
let isCall = 1, Defs = [R24, R25, R18, R19, R20, R21, R22, R23,
                        R30, R31, R26, R27] in {
*/
let isCall = 1 in {
// Long Call to a Subroutine
  def CALL : ProgDirAddr<0b1001010111,(outs),(ins calltarget:$dst),
           "call $dst",
           []>;
}
//===----------------------------------------------------------------------===//
// Data Transfer instructions
//===----------------------------------------------------------------------===//

// Copy Register
def MOV  :  DirAddrTwoReg<0b001011, (outs GPRegs:$dst), (ins GPRegs:$src),
            "mov $dst, $src",
            []>;
//            [(set GPRegs:$dst, GPRegs:$src)]>;

// Copy Register Pair
def MOVW : DirAddrTwoWReg<0b00000001,(outs WRegs:$dst),(ins WRegs:$src),
           "movw $dst, $src",
           []>;
//           [(set WRegs:$dst, WRegs:$src)]>;

// FIXME: some kind of multiclasses should be used to define these instruction.
// load functions
let mayLoad = 1 in {
let Uses = [X_PTR] in {
  def LDx   : DirAddrSingleReg<0b10010001100,(outs GPRegs:$dst),(ins ),
            "ld $dst, X",
            []>;
  let Defs = [X_PTR] in {
  def LDxpi : DirAddrSingleReg<0b10010001101,(outs GPRegs:$dst),(ins ),
            "ld $dst, X+",
            []>;
  def LDxpd : DirAddrSingleReg<0b10010001110,(outs GPRegs:$dst),(ins ),
            "ld $dst, -X",
            []>;
  }
}

let Uses = [Y_PTR] in {
  def LDy   : DirAddrSingleReg<0b10000001000,(outs GPRegs:$dst),(ins ),
            "ld $dst, Y",
            []>;
  let Defs = [Y_PTR] in {
  def LDypi : DirAddrSingleReg<0b10010001001,(outs GPRegs:$dst),(ins ),
            "ld $dst, Y+",
            []>;
  def LDypd : DirAddrSingleReg<0b10010001010,(outs GPRegs:$dst),(ins ),
            "ld $dst, -Y",
            []>;
  }
  def LDDy  : DataIndirDisp<0b10001,(outs GPRegs:$dst),(ins i8imm:$src),
            "ldd $dst, Y+$src",
            []>;
}

let Uses = [Z_PTR] in {
  def LDz   : DirAddrSingleReg<0b10000000000,(outs GPRegs:$dst),(ins ),
            "ld $dst, Z",
            []>;
  let Defs = [Z_PTR] in {
  def LDzpi : DirAddrSingleReg<0b10010000001,(outs GPRegs:$dst),(ins ),
            "ld $dst, Z+",
            []>;
  def LDzpd : DirAddrSingleReg<0b10010000010,(outs GPRegs:$dst),(ins ),
            "ld $dst, -Z",
            []>;
  }
  def LDDz  : DataIndirDisp<0b10000,(outs GPRegs:$dst),(ins i8imm:$src),
            "ldd $dst, Z+$src",
            []>;
}
} // mayLoad = 1

// Store instructions
let mayStore = 1 in {
let Uses = [X_PTR] in {
  def STx   : DirAddrSingleReg<0b10010011100,(outs GPRegs:$dst),(ins ),
            "st $dst, X",
            []>;
  let Defs = [X_PTR] in {
  def STxpi : DirAddrSingleReg<0b10010011101,(outs GPRegs:$dst),(ins ),
            "st $dst, X+",
            []>;
  def STxpd : DirAddrSingleReg<0b10010011110,(outs GPRegs:$dst),(ins ),
            "st $dst, -X",
            []>;
  }
}

let Uses = [Y_PTR] in {
  def STy   : DirAddrSingleReg<0b10000011000,(outs GPRegs:$dst),(ins ),
            "st $dst, Y",
            []>;
  let Defs = [Y_PTR] in {
  def STypi : DirAddrSingleReg<0b10010011001,(outs GPRegs:$dst),(ins ),
            "st $dst, Y+",
            []>;
  def STypd : DirAddrSingleReg<0b10010011010,(outs GPRegs:$dst),(ins ),
            "st $dst, -Y",
            []>;
  }
  def STDy  : DataIndirDisp<0b10011,(outs GPRegs:$dst),(ins i8imm:$src),
            "std $dst, Y+$src",
            []>;
}

let Uses = [Z_PTR] in {
  def STz   : DirAddrSingleReg<0b10000010000,(outs GPRegs:$dst),(ins ),
            "st $dst, Z",
            []>;
  let Defs = [Z_PTR] in {
  def STzpi : DirAddrSingleReg<0b10010010001,(outs GPRegs:$dst),(ins ),
            "st $dst, Z+",
            []>;
  def STzpd : DirAddrSingleReg<0b10010010010,(outs GPRegs:$dst),(ins ),
            "st $dst, -Z",
            []>;
  }
  def STDz  : DataIndirDisp<0b10010,(outs GPRegs:$dst),(ins i8imm:$src),
            "std $dst, Z+$src",
            []>;
}
} // mayStore

// Addressing modes
def IOAddr :ComplexPattern<iPTR, 1, "SelectIOAddr", [], []>;

// Memory operands
def MEMri : Operand<i8> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GPRegs, i16imm);
}

// In From I/O Location
def IN  : IODirAddr<0b10110,(outs GPRegs:$dst),(ins MEMri:$src),
         "in $dst, $src",
         [(set GPRegs:$dst, (load IOAddr:$src))]>;
// Out To I/O Location
def OUT : IODirAddr<0b10111,(outs MEMri:$dst),(ins GPRegs:$src),
         "out $dst, $src",
         [(store GPRegs:$src, IOAddr:$dst)]>;
let Defs = [SP] in {
// Push Register on Stack
def PUSH : DirAddrSingleReg<0b10010011111,(outs),(ins GPRegs:$src),
         "push $src",
         []>;
// Pop Register from Stack
def POP  : DirAddrSingleReg<0b10010001111,(outs GPRegs:$dst),(ins),
         "pop $dst",
         []>;
}
//===----------------------------------------------------------------------===//
// Bit and bit-test instructions
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// MCU control instructions
//===----------------------------------------------------------------------===//

// NOP
let neverHasSideEffects = 1 in
  def NOP : Pseudo<0x00, (outs), (ins), "nop", []>;
// SLEEP
// WDR
// BREAK


