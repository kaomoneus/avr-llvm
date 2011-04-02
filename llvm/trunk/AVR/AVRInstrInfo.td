//===- AVRInstrInfo.td - Target Description for AVR Target -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the AVR instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "AVRInstrFormats.td"

//:TODO: NOTE HERE!!
// "defs" = instruction "uses"

// :NOTE: all instructions have their properties set explicitly to make thigs
// clearer.

//===----------------------------------------------------------------------===//
// Feature predicates.
//===----------------------------------------------------------------------===//
//:TODO: enable this once we get into subtarget support

//def HasJMP   : Predicate<"Subtarget.hasJMP()">;
//def HasMUL   : Predicate<"Subtarget.hasMUL()">;
//def HasMOVW  : Predicate<"Subtarget.hasMOVW()">;
//def HasJTAG  : Predicate<"Subtarget.hasJTAG()">;
//def HasEIJMP : Predicate<"Subtarget.hasEIJMP()">;
//def HasSPM   : Predicate<"Subtarget.hasSPM()">;
//def HasDES   : Predicate<"Subtarget.hasDES()">;
//def HasEIJMP_EICALL : Predicate<"Subtarget.has
//def HasELPM : Predicate<"Subtarget.has
//def HasELPMX : Predicate<"Subtarget.has
//def HasJMP_CALL : Predicate<"Subtarget.has
//def HasMOVW_LPMX : Predicate<"Subtarget.has
//def HasRAMPX_Y_D : Predicate<"Subtarget.has
//def HasTiny_Regs_Insns : Predicate<"Subtarget.has
//def HasCoreExt : Predicate<"Subtarget.has

//def IsAVR2   : Predicate<"Subtarget.isAVR2()">;
//def IsAVR25  : Predicate<"Subtarget.isAVR25()">;
//def IsAVR3   : Predicate<"Subtarget.isAVR3()">;
//def IsAVR31  : Predicate<"Subtarget.isAVR31()">;
//def IsAVR4   : Predicate<"Subtarget.isAVR4()">;
//def IsAVR5   : Predicate<"Subtarget.isAVR5()">;
//def IsAVR51  : Predicate<"Subtarget.isAVR51()">;
//def IsAVR6   : Predicate<"Subtarget.isAVR6()">;
//def IsXMEGA  : Predicate<"Subtarget.isXMEGA()">;

//===----------------------------------------------------------------------===//
// AVR Condition Codes
//===----------------------------------------------------------------------===//
//:TODO: enable this once we implement branching

/*/// test on SREG flags
def AVR_COND_C  : PatLeaf<(i8 0)>; // Carry Flag
def AVR_COND_Z  : PatLeaf<(i8 1)>; // Zero Flag
def AVR_COND_N  : PatLeaf<(i8 2)>; // Negative Flag
def AVR_COND_V  : PatLeaf<(i8 3)>; // Two's complement overflow indicator
def AVR_COND_S  : PatLeaf<(i8 4)>; // Sign Test
def AVR_COND_H  : PatLeaf<(i8 5)>; // Half Carry Flag
def AVR_COND_T  : PatLeaf<(i8 6)>; // Transfer bit
def AVR_COND_I  : PatLeaf<(i8 7)>; // Global Interupt*/
/*
/// conditional tests
/// bit is set
def AVR_COND_E  : PatLeaf<(i8 1)>; // Equal
def AVR_COND_LO : PatLeaf<(i8 0)>; // Lower (unsigned)
def AVR_COND_LT : PatLeaf<(i8 4)>; // Less Then (signed)
def AVR_COND_MI : PatLeaf<(i8 2)>; // Minus
/// bit is cleared
def AVR_COND_GE : PatLeaf<(i8 4)>; // Greater or Equal (signed)
def AVR_COND_NE : PatLeaf<(i8 1)>; // Not Equal
def AVR_COND_PL : PatLeaf<(i8 2)>; // Plus
def AVR_COND_SH : PatLeaf<(i8 0)>; // Same or higher (unsigned)
*/

//===----------------------------------------------------------------------===//
// Type Profiles.
//===----------------------------------------------------------------------===//

def SDT_AVRCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_AVRCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def SDT_AVRCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_AVRWrapper : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;

//===----------------------------------------------------------------------===//
// AVR Specific Node Definitions
//===----------------------------------------------------------------------===//

def AVRretflag : SDNode<"AVRISD::RET_FLAG", SDTNone,
                        [SDNPHasChain, SDNPOptInGlue]>;

def AVRcallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_AVRCallSeqStart,
                              [SDNPHasChain, SDNPOutGlue]>;

def AVRcallseq_end : SDNode<"ISD::CALLSEQ_END", SDT_AVRCallSeqEnd,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def AVRcall : SDNode<"AVRISD::CALL", SDT_AVRCall,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

def AVRWrapper : SDNode<"AVRISD::Wrapper", SDT_AVRWrapper>;

def AVRlsl : SDNode<"AVRISD::LSL", SDTIntUnaryOp>;
def AVRlsr : SDNode<"AVRISD::LSR", SDTIntUnaryOp>;
def AVRrol : SDNode<"AVRISD::ROL", SDTIntUnaryOp>;
def AVRror : SDNode<"AVRISD::ROR", SDTIntUnaryOp>;
def AVRasr : SDNode<"AVRISD::ASR", SDTIntUnaryOp>;

// :TODO: TEMPORARY INSTRUCTION LISTING, leave it here until we're done with it
// Arithmetic instructions
/*
  ADD RD, RR
  ADC RD, RR
  ADIW RD, K

  SUB RD, RR
  SUBI RD, K
  SBC RD, RR
  SBCI RD, K
  SBIW RD, K

  INC RD
  DEC RD

  MUL RD, RR
  MULS RD, RR
  MULSU RD, RR
  FMUL RD, RR
  FMULS RD, RR
  FMULSU RD, RR

  DES??
*/

/*
def ADDrr       0000 11rd dddd rrrr ok
def ADCrr       0001 11rd dddd rrrr ok
def ADIWri      1001 0110 KKdd KKKK ok

def SUBrr       0001 10rd dddd rrrr ok
def SUBIri      0101 KKKK dddd KKKK ok
def SBCrr       0000 10rd dddd rrrr ok
def SBCIri      0100 KKKK dddd KKKK ok
def SBIWri      1001 0111 KKdd KKKK ok

def INCr        1001 010d dddd 0011 ok
def DECr        1001 010d dddd 1010 ok

def MULrr       1001 11rd dddd rrrr ok
def MULSrr      0000 0010 dddd rrrr ok
def MULSUrr     0000 0011 0ddd 0rrr ok
def FMULrr      0000 0011 0ddd 1rrr ok
def FMULSrr     0000 0011 1ddd 0rrr ok
def FMULSUrr    0000 0011 1ddd 1rrr ok
*/
//def DES :TODO:

// Logic instructions
/*
  AND RD, RR
  ANDI RD, K

  OR RD, RR
  ORI RD, K

  EOR RD, RR

  COM RD
  NEG RD

  SBR RD, K
  CBR RD, K

  TST RD

  CLR RD

  SER RD
*/
/*
def ANDrr           0010 00rd dddd rrrr ok
def ANDIri          0111 KKKK dddd KKKK ok
def ORrr            0010 10rd dddd rrrr ok
def ORIri           0110 KKKK dddd KKKK ok
def EORrr           0010 01rd dddd rrrr ok
def COMr            1001 010d dddd 0000 ok
def NEGr            1001 010d dddd 0001 ok
def SBRri           0110 KKKK dddd KKKK ok
def CBRri           SAME AS   ANDI      ok
def TSTr            0010 00dd dddd dddd (SAME AS AND RD, RD) ok
def CLRr            0010 01dd dddd dddd (SAME AS EOR RD, RD) ok
def SERr            1110 1111 dddd 1111 (SAME AS LDI RD, 0xFF) ok

// Branch instructions
def RJMPi           1100 kkkk kkkk kkkk ok  //:TODO: use a for addr instead of i?
def IJMP            1001 0100 0000 1001 ok
def JMPi            1001 010k kkkk 110k kkkk kkkk kkkk kkkk

def RCALLi          1101 kkkk kkkk kkkk ok
def ICALL           1001 0101 0000 1001 ok
def EICALL          1001 0101 0001 1001 ok
def CALLi           1001 010k kkkk 111k kkkk kkkk kkkk kkkk
def RET             1001 0101 0000 1000 ok
def RETI            1001 0101 0001 1000 ok

def CPSErr          0001 00rd dddd rrrr ok
def CPrr            0001 01rd dddd rrrr ok
def CPCrr           0000 01rd dddd rrrr ok
def CPIri           0011 kkkk dddd kkkk ok

def SBRCrb          1111 110r rrrr 0bbb
def SBRSrb          1111 111r rrrr 0bbb
def SBICab          1001 1001 AAAA Abbb
def SBISab          1001 1011 AAAA Abbb

def BRBSsi          1111 00kk kkkk ksss ok
def BRBCsi          1111 01kk kkkk ksss ok

def BREQi           1111 00kk kkkk k001 ok
def BRNEi           1111 01kk kkkk k001 ok
def BRCSi           1111 00kk kkkk k000 ok
def BRCCi           1111 01kk kkkk k000 ok
def BRSHi           1111 01kk kkkk k000 ok
def BRLOi           1111 00kk kkkk k000 ok
def BRMIi           1111 00kk kkkk k010 ok
def BRPLi           1111 01kk kkkk k010 ok
def BRGEi           1111 01kk kkkk k100 ok
def BRLTi           1111 00kk kkkk k100 ok
def BRHSi           1111 00kk kkkk k101 ok
def BRHCi           1111 01kk kkkk k101 ok
def BRTSi           1111 00kk kkkk k110 ok
def BRTCi           1111 01kk kkkk k110 ok
def BRVSi           1111 00kk kkkk k011 ok
def BRVCi           1111 01kk kkkk k011 ok
def BRIEi           1111 00kk kkkk k111 ok
def BRIDi           1111 01kk kkkk k111 ok

// data transfer instructions
def MOVrr           0010 11rd dddd rrrr ok
def MOVWrr          0000 0001 dddd rrrr

def LDIri           1110 kkkk dddd kkkk ok
def LDSri           1001 000d dddd 0000 kkkk kkkk kkkk kkkk

def LDrX            1001 000d dddd 1100
def LDrXpi          1001 000d dddd 1101     //postinc
def LDrXpd          1001 000d dddd 1110//predec

def LDrY            1000 000d dddd 1000
def LDrYpi          1001 000d dddd 1001
def LDrYpd          1001 000d dddd 1010

def LDDrYq          10q0 qq0d dddd 1qqq

def LDrZ            1000 000d dddd 0000
def LDrZpi          1001 000d dddd 0001
def LDrZpd          1001 000d dddd 0010

def LDDrZq          10q0 qq0d dddd 0qqq

def STSir           1001 001d dddd 0000 kkkk kkkk kkkk kkkk
def STXr            1001 001d dddd 1100     // watch out: r cant be X
def STXpir          1001 001d dddd 1101
def STXpdr          1001 001d dddd 1110

def STYr            1000 001d dddd 1000     // watch out: r cant Y
def STYpir          1001 001d dddd 1001
def STYpdr          1001 001d dddd 1010
def STDYqr          10q0 qq1d dddd 1qqq

def STZr            1000 001d dddd 0000
def STZpir          1001 001d dddd 0001
def STZpdr          1001 001d dddd 0010
def STDqr           10q0 qq1d dddd 0qqq

def LPM             1001 0101 1100 1000     //(R0)<-(Z)
def LPMrZ           1001 000d dddd 0100
def LPMrZpi         1001 000d dddd 0101

def ELPM            1001 0101 1101 1000
def ELPMrZ          1001 000d dddd 0110
def ELPMrZpi        1001 000d dddd 0111

def SPM             //:TODO:
def SPMZpi

def INra            1011 0AAd dddd AAAA ok
def OUTar           1011 1AAd dddd AAAA ok

def PUSHr           1001 001d dddd 1111 ok
def POPr            1001 000d dddd 1111 ok

//bit and bit-test instructions
def LSLr            0000 11dd dddd dddd   ok  (SAME AS ADD rd, rd)
def LSRr            1001 010d dddd 0110   ok
def ROLr            0001 11dd dddd dddd   ok  (SAME AS ADC rd, rd)
def RORr            1001 010d dddd 0111   ok

def ASRr            1001 010d dddd 0101  ok

def SWAPr           1001 010d dddd 0010 ok

def SBIab           1001 1010 AAAA Abbb
def CBIab           1001 1000 AAAA Abbb

def BSTrb           1111 101d dddd 0bbb
def BLDrb           1111 100d dddd 0bbb

def BSETs           1001 0100 0sss 1000
def BCLRs           1001 0100 1sss 1000 // probably all the todo below comes from here
//:TODO: all SREG instr manipulation (SEC, CLC, SEN, etc...)

// MCU control instructions
def NOP             0000 0000 0000 0000
*/
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________

//:TODO: INSTRUCTION pattern implementation
// To implement each instruction just uncomment it and remove it from the list.
// Instructions are listed exactly the same way as in the AVR ins set datahseet

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber SREG.
//let Defs = [SPW, SRW], Uses = [SPW] in //:TODO: readd this once SP is implemented
let Uses = [R5] in
{
def ADJCALLSTACKDOWN : Pseudo<(outs),
                              (ins i16imm:$amt),
                              "#ADJCALLSTACKDOWN",
                              [(AVRcallseq_start timm:$amt)]>;

def ADJCALLSTACKUP : Pseudo<(outs),
                            (ins i16imm:$amt1, i16imm:$amt2),
                            "#ADJCALLSTACKUP",
                            [(AVRcallseq_end timm:$amt1, timm:$amt2)]>;
}

//:TODO: define all flags affected per instruction!

// real instructions
// Arithmetic
let isCommutable = 1,
Constraints = "$src = $dst",
Defs = [SREG] in
def ADDRdRr : FRdRr<0b0000,
                    0b11,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "add\t$dst, $src2",
                    [(set GPR8:$dst, (add GPR8:$src, GPR8:$src2)),
                     (implicit SREG)]>;

let isCommutable = 1,
Constraints = "$src = $dst",
Defs = [SREG],
Uses = [SREG] in
def ADCRdRr : FRdRr<0b0001,
                    0b11,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "adc\t$dst, $src2",
                    [(set GPR8:$dst, (adde GPR8:$src, GPR8:$src2)),
                     (implicit SREG)]>;

/*def ADIW
*/

let Constraints = "$src = $dst" in
def SUBRdRr : FRdRr<0b0001,
                    0b10,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "sub\t$dst, $src2",
                    [(set GPR8:$dst, (sub GPR8:$src, GPR8:$src2))]>;

let Constraints = "$src = $dst" in
def SUBIRdK : FRdK<0b0101,
                   (outs LDREGS:$dst),
                   (ins LDREGS:$src, i8imm:$src2),
                   "subi\t$dst, $src2",
                   [(set LDREGS:$dst, (sub LDREGS:$src, imm:$src2))]>;

let Constraints = "$src = $dst" in
def SBCRdRr : FRdRr<0b0000,
                    0b10,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "sbc\t$dst, $src2",
                    [(set GPR8:$dst, (sube GPR8:$src, GPR8:$src2))]>;

let Constraints = "$src = $dst" in
def SBCIRdK : FRdK<0b0100,
                   (outs LDREGS:$dst),
                   (ins LDREGS:$src, i8imm:$src2),
                   "sbci\t$dst, $src2",
                   [(set LDREGS:$dst, (sube LDREGS:$src, imm:$src2))]>;

/*def SBIWri
*/
let Constraints = "$src = $dst" in
def INCRd : FRd<0b1001,
                0b0100011,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "inc\t$dst",
                [(set GPR8:$dst, (add GPR8:$src, 1))]>;

let Constraints = "$src = $dst" in
def DECRd : FRd<0b1001,
                0b0101010,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "dec\t$dst",
                [(set GPR8:$dst, (add GPR8:$src, -1))]>;

//:TODO: this is an unsigned mult!!
//:TODO: xor r1 after multiplication
let neverHasSideEffects = 1,
Defs = [R1, R0] in
def MULRdRr : Mu1RdRr<(outs),
                      (ins GPR8:$src1, GPR8:$src2),
                      "mul\t$src1, $src2",
                      []>;

/*def MULSrr      0000 0010 dddd rrrr ok
def MULSUrr     0000 0011 0ddd 0rrr ok
def FMULrr      0000 0011 0ddd 1rrr ok
def FMULSrr     0000 0011 1ddd 0rrr ok
def FMULSUrr    0000 0011 1ddd 1rrr ok
*/
//_______________________________________
// logic instructions
let isCommutable = 1,
Constraints = "$src = $dst" in
def ANDRdRr : FRdRr<0b0010,
                    0b00,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "and\t$dst, $src2",
                    [(set GPR8:$dst, (and GPR8:$src, GPR8:$src2))]>;

let Constraints = "$src = $dst" in
def ANDIRdK : FRdK<0b0111,
                   (outs LDREGS:$dst),
                   (ins LDREGS:$src, i8imm:$src2),
                   "andi\t$dst, $src2",
                   [(set LDREGS:$dst, (and LDREGS:$src, imm:$src2))]>;

let isCommutable = 1,
Constraints = "$src = $dst" in
def ORRdRr : FRdRr<0b0010,
                   0b10,
                   (outs GPR8:$dst),
                   (ins GPR8:$src, GPR8:$src2),
                   "or\t$dst, $src2",
                   [(set GPR8:$dst, (or GPR8:$src, GPR8:$src2))]>;

let Constraints = "$src = $dst" in
def ORIRdK : FRdK<0b0110,
                  (outs LDREGS:$dst),
                  (ins LDREGS:$src, i8imm:$src2),
                  "ori\t$dst, $src2",
                  [(set LDREGS:$dst, (or LDREGS:$src, imm:$src2))]>;

let isCommutable = 1,
Constraints = "$src = $dst" in
def EORRdRr : FRdRr<0b0010,
                    0b01,
                    (outs GPR8:$dst),
                    (ins GPR8:$src, GPR8:$src2),
                    "eor\t$dst, $src2",
                    [(set GPR8:$dst, (xor GPR8:$src, GPR8:$src2))]>;

let Constraints = "$src = $dst" in
def COMRd : FRd<0b1001,
                0b0100000,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "com\t$dst",
                [(set GPR8:$dst, (not GPR8:$src))]>;

let Constraints = "$src = $dst" in
def NEGRd : FRd<0b1001,
                0b0100001,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "neg\t$dst",
                [(set GPR8:$dst, (ineg GPR8:$src))]>;

/*def SBRri           0110 KKKK dddd KKKK ok
def CBRri           SAME AS   ANDI      ok
def TSTr            0010 00dd dddd dddd (SAME AS AND RD, RD) ok
def CLRr            0010 01dd dddd dddd (SAME AS EOR RD, RD) ok
def SERr            1110 1111 dddd 1111 (SAME AS LDI RD, 0xFF) ok
___________________________________________________________________________________
// Branch instructions
def RJMPi           1100 kkkk kkkk kkkk ok  //:TODO: use a for addr instead of i?
def IJMP            1001 0100 0000 1001 ok
def JMPi            1001 010k kkkk 110k kkkk kkkk kkkk kkkk

def RCALLi          1101 kkkk kkkk kkkk ok
def ICALL           1001 0101 0000 1001 ok
def EICALL          1001 0101 0001 1001 ok
def CALLi           1001 010k kkkk 111k kkkk kkkk kkkk kkkk*/

let isCall = 1 in
  // All calls clobber the non-callee saved registers. SP is marked as
  // a use to prevent stack-pointer assignments that appear immediately
  // before calls from potentially appearing dead. Uses for argument
  // registers are added manually.
  let Defs = [R0], //:TODO: re-add all regs!!
  Uses = [R4] in //:TODO: add the stack ptr here
  {
      def CALLk : F32BRk<0b111,
                         (outs),
      //:TODO: the imm field can be either 16 or 22 bits in devices with more
      // than 64k of ROM, fix it once we support the largests devices.
                         (ins i16imm:$dst, variable_ops),
                         "call\t$dst",
                         [(AVRcall imm:$dst)]>;
  }

let isTerminator = 1,
isReturn = 1,
isBarrier = 1,
hasCtrlDep = 1 in
def RET : FBR<0b1001010100001000,
              (outs),
              (ins),
              "ret",
              [(AVRretflag)]>;
/*
def RETI            1001 0101 0001 1000 ok

def CPSErr          0001 00rd dddd rrrr ok
def CPrr            0001 01rd dddd rrrr ok
def CPCrr           0000 01rd dddd rrrr ok
def CPIri           0011 kkkk dddd kkkk ok

def SBRCrb          1111 110r rrrr 0bbb
def SBRSrb          1111 111r rrrr 0bbb
def SBICab          1001 1001 AAAA Abbb
def SBISab          1001 1011 AAAA Abbb

def BRBSsi          1111 00kk kkkk ksss ok
def BRBCsi          1111 01kk kkkk ksss ok

def BREQi           1111 00kk kkkk k001 ok
def BRNEi           1111 01kk kkkk k001 ok
def BRCSi           1111 00kk kkkk k000 ok
def BRCCi           1111 01kk kkkk k000 ok
def BRSHi           1111 01kk kkkk k000 ok
def BRLOi           1111 00kk kkkk k000 ok
def BRMIi           1111 00kk kkkk k010 ok
def BRPLi           1111 01kk kkkk k010 ok
def BRGEi           1111 01kk kkkk k100 ok
def BRLTi           1111 00kk kkkk k100 ok
def BRHSi           1111 00kk kkkk k101 ok
def BRHCi           1111 01kk kkkk k101 ok
def BRTSi           1111 00kk kkkk k110 ok
def BRTCi           1111 01kk kkkk k110 ok
def BRVSi           1111 00kk kkkk k011 ok
def BRVCi           1111 01kk kkkk k011 ok
def BRIEi           1111 00kk kkkk k111 ok
def BRIDi           1111 01kk kkkk k111 ok
________________________________________________________________________________
// data transfer instructions
*/

let neverHasSideEffects = 1 in
def MOVRdRr : FRdRr<0b0010,
                    0b11,
                    (outs GPR8:$dst),
                    (ins GPR8:$src),
                    "mov\t$dst, $src",
                    []>;

let neverHasSideEffects = 1 in
def MOVWRdRr : FMOVWRdRr<(outs GPR8_E:$dst),
                         (ins GPR8_E:$src),
                         "movw\t$dst, $src",
                         []>;

let isReMaterializable = 1,
isAsCheapAsAMove = 1 in
def LDIRdK : FRdK<0b1110,
                  (outs LDREGS:$dst),
                  (ins i8imm:$src),
                  "ldi\t$dst, $src",
                  [(set LDREGS:$dst, imm:$src)]>;
/*
def LDSri           1001 000d dddd 0000 kkkk kkkk kkkk kkkk

def LDrX            1001 000d dddd 1100
def LDrXpi          1001 000d dddd 1101     //postinc
def LDrXpd          1001 000d dddd 1110//predec

def LDrY            1000 000d dddd 1000
def LDrYpi          1001 000d dddd 1001
def LDrYpd          1001 000d dddd 1010

def LDDrYq          10q0 qq0d dddd 1qqq

def LDrZ            1000 000d dddd 0000
def LDrZpi          1001 000d dddd 0001
def LDrZpd          1001 000d dddd 0010

def LDDrZq          10q0 qq0d dddd 0qqq

def STSir           1001 001d dddd 0000 kkkk kkkk kkkk kkkk
def STXr            1001 001d dddd 1100     // watch out: r cant be X
def STXpir          1001 001d dddd 1101
def STXpdr          1001 001d dddd 1110

def STYr            1000 001d dddd 1000     // watch out: r cant Y
def STYpir          1001 001d dddd 1001
def STYpdr          1001 001d dddd 1010
def STDYqr          10q0 qq1d dddd 1qqq

def STZr            1000 001d dddd 0000
def STZpir          1001 001d dddd 0001
def STZpdr          1001 001d dddd 0010
def STDqr           10q0 qq1d dddd 0qqq

def LPM             1001 0101 1100 1000     //(R0)<-(Z)
def LPMrZ           1001 000d dddd 0100
def LPMrZpi         1001 000d dddd 0101

def ELPM            1001 0101 1101 1000
def ELPMrZ          1001 000d dddd 0110
def ELPMrZpi        1001 000d dddd 0111

def SPM             //:TODO:
def SPMZpi

def INra            1011 0AAd dddd AAAA ok
def OUTar           1011 1AAd dddd AAAA ok
*/

//let Defs = [SP], //:TODO: readd this
//Uses = [SP],
let neverHasSideEffects = 1,
mayStore = 1 in
def PUSHRr : FRd<0b1001,
                 0b0011111,
                 (outs),
                 (ins GPR8:$reg),
                 "push\t$reg",
                 []>;

//let Defs = [SP],
//Uses = [SP], //:TODO: readd this
let neverHasSideEffects = 1,
mayLoad = 1 in
def POPRd : FRd<0b1001,
                0b0001111,
                (outs GPR8:$reg),
                (ins),
                "pop\t$reg",
                []>;
/*__________________________________________________________________________________
//bit and bit-test instructions
*/
let Constraints = "$src = $dst"  in
def LSLRd : FRdRr<0b0000,
                  0b11,
                  (outs GPR8:$dst),
                  (ins GPR8:$src),
                  "lsl\t$dst",
                  [(set GPR8:$dst, (AVRlsl GPR8:$src))]>;

let Constraints = "$src = $dst" in
def LSRRd : FRd<0b1001,
                0b0100110,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "lsr\t$dst",
                [(set GPR8:$dst, (AVRlsr GPR8:$src))]>;

let Constraints = "$src = $dst" in
def ROLRd : FRdRr<0b0001,
                  0b11,
                  (outs GPR8:$dst),
                  (ins GPR8:$src),
                  "rol\t$dst",
                  [(set GPR8:$dst, (AVRrol GPR8:$src))]>;

let Constraints = "$src = $dst" in
def RORRd : FRd<0b1001,
                0b0100111,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "ror\t$dst",
                [(set GPR8:$dst, (AVRror GPR8:$src))]>;

let Constraints = "$src = $dst" in
def ASRRd : FRd<0b1001,
                0b0100101,
                (outs GPR8:$dst),
                (ins GPR8:$src),
                "asr\t$dst",
                [(set GPR8:$dst, (AVRasr GPR8:$src))]>;

let Constraints = "$src = $dst" in
def SWAPRd : FRd<0b1001,
                 0b0100010,
                 (outs GPR8:$dst),
                 (ins GPR8:$src),
                 "swap\t$dst",
                 [(set GPR8:$dst, (bswap GPR8:$src))]>;
/*
def SBIab           1001 1010 AAAA Abbb
def CBIab           1001 1000 AAAA Abbb

def BSTrb           1111 101d dddd 0bbb
def BLDrb           1111 100d dddd 0bbb

def BSETs           1001 0100 0sss 1000
def BCLRs           1001 0100 1sss 1000 // probably all the todo below comes from here
//:TODO: all SREG instr manipulation (SEC, CLC, SEN, etc...)
__________________________________________________________________________________
// MCU control instructions
def NOP             0000 0000 0000 0000
//TODO: rest
*/

//:TODO: make add reg, imm -> sub reg, -imm

def : Pat<(addc GPR8:$src, GPR8:$src2),
          (ADDRdRr GPR8:$src, GPR8:$src2)>;
def : Pat<(subc GPR8:$src, GPR8:$src2),
          (SUBRdRr GPR8:$src, GPR8:$src2)>;

//def : Pat<(i16 (AVRWrapper tglobaladdr:$dst)), (MOVWRdRr1 tglobaladdr:$dst)>;
def : Pat<(add GPR8:$src, (AVRWrapper tglobaladdr:$src2)),
          (ADDRdRr GPR8:$src, tglobaladdr:$src2)>;

def : Pat<(AVRcall tglobaladdr:$dst),
          (CALLk tglobaladdr:$dst)>;

/*
def : Pattern<(or WDREGS:$src1, WDREGS:$src2),
          [(ORRdRr (EXTRACT_SUBREG WDREGS:$src1, AVR_subreg_8bit),
          (EXTRACT_SUBREG WDREGS:$src2, AVR_subreg_8bit)),
          (ORRdRr (EXTRACT_SUBREG WDREGS:$src1, AVR_subreg_8bit_hi),
          (EXTRACT_SUBREG WDREGS:$src2, AVR_subreg_8bit_hi))]>;*/

/*
// trunc patterns
def : Pat<(i8 (trunc WDREGS:$src)),
          (EXTRACT_SUBREG WDREGS:$src, AVR_subreg_8bit)>;
def : Pat<(i16 (anyext GPR8:$src)),
          (SUBREG_TO_REG (i16 0), GPR8:$src, AVR_subreg_8bit)>;
def def8 : PatLeaf<(i8 GPR8:$src), [{
  return N->getOpcode() != ISD::TRUNCATE &&
         N->getOpcode() != TargetOpcode::EXTRACT_SUBREG &&
         N->getOpcode() != ISD::CopyFromReg;
}]>;
def : Pat<(i16 (zext def8:$src)),
          (SUBREG_TO_REG (i16 0), GPR8:$src, AVR_subreg_8bit)>;*/
