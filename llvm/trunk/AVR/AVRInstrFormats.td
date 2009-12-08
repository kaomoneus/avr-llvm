//===- AVRInstrFormats.td - AVR Instruction Formats ----------*- tblgen -*-===//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  AVR Instruction Set Nomenclature
//
//  Registers and Operands:
//
//  op    - operation code (all bits).
//  Rd    - Destination (and source) register
//  Rr    - Source register 
//  w     - Arithmetic Word register (R24, R26, R28, R30)
//  K     - Constant data
//  k     - Constant address
//  b     - Bit in the Register File or I/O Register (3-bit)
//  s     - Bit in the Status Register
//  X,Y,Z - Indirect Address Register
//  A     - I/O location address (0-64, 6-bit)
//  a     - I/O location address (bit accessable)  (0-31, 5 bit)
//  q     - Displacement for direct addressing (6-bit)
//
//===----------------------------------------------------------------------===//

// Generic AVR Format
class AVRInst<dag outs, dag ins, string asmstr, list<dag> pattern>
        : Instruction 
{
  let Namespace = "AVR";

  dag OutOperandList = outs;
  dag InOperandList = ins;

  let AsmString = asmstr;
  let Pattern = pattern;
}

// AVR word instruction
class WordInst<dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :AVRInst<outs, ins, asmstr, pattern> 
{
  field bits<16> Instr;
}

// AVR double word instruction
class DoubleWordInst<dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :AVRInst<outs, ins, asmstr, pattern> 
{
  field bits<32> Instr;
}

// Pseudo Instruction
class Pseudo<bits<16> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  let Instr = op; 
}
// NOTE: the folling addressing modes are taken from the AVR Instruction Set
//       (0856G-AVR-07/08). The positions of the addressing bits in the 
//       figures are simplyfied like in the Instruction Set reference.

//===----------------------------------------------------------------------===/
// direct single register addressing              |   op    | Rd |
//===----------------------------------------------------------------------===/
class DirAddrSingleReg<bits<11> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{ 
  bits<5> Rd;

  let Instr{15-9} = op{10-4};
  let Instr{8-4} = Rd;
  let Instr{3-0} = op{3-0};
}

//===----------------------------------------------------------------------===/
// direct register register addressing,two regs   | op  | Rr | Rd|
//===----------------------------------------------------------------------===/
class DirAddrTwoReg<bits<6> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<5> Rr;
  bits<5> Rd;

  let Instr{15-10} = op;
  let Instr{9} = Rd{4};
  let Instr{8-4} = Rr;
  let Instr{3-0} = Rd{3-0};
}
  
//===----------------------------------------------------------------------===/
// direct Register Register addressing,two wregs  | op    |Rr |Rd|
//===----------------------------------------------------------------------===/
class DirAddrTwoWReg<bits<8> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<4> Rr;
  bits<4> Rd;

  let Instr{15-8} = op;
  let Instr{7-4} = Rr;
  let Instr{3-0} = Rd;
}

//===----------------------------------------------------------------------===/
// direct Register Register addressing, two FmulRegs | op     |Rr|Rd|
//===----------------------------------------------------------------------===/
class DirAddrTwoFmulReg<bits<10> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<3> Rr;
  bits<3> Rd;

  let Instr{15-7} = op{9-1};
  let Instr{6-4}  = Rr;
  let Instr{3}    = op{0};
  let Instr{2-0}  = Rd;
}

//===----------------------------------------------------------------------===/
// IO direct addressing                           | op | Rr | A  |
//===----------------------------------------------------------------------===/
class IODirAddr<bits<5> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<5> Rr;
  bits<6> A;

  let Instr{15-11} = op;
  let Instr{10-9} = A{5-4};
  let Instr{8-4} = Rr;
  let Instr{3-0} = A{3-0};
}

//===----------------------------------------------------------------------===/
// Direct Data Addressing                         |    op   | Rr |
//                                                | Data Addr(k) |
//===----------------------------------------------------------------------===/
class DataDirAddr<bits<11> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :DoubleWordInst<outs, ins, asmstr, pattern> 
{
  bits<5> Rr;
  bits<16> k;

  let Instr{31-25} = op{10-4};
  let Instr{24-20} = Rr;
  let Instr{19-16} = op{3-0};
  let Instr{15-0}  = k;
}

//===----------------------------------------------------------------------===/
// Direct Program Addressing (JMP, CALL)          |    op   | k  |
//                                                |       k      |
//===----------------------------------------------------------------------===/
class ProgDirAddr<bits<10> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :DoubleWordInst<outs, ins, asmstr, pattern> 
{
  bits<22> k;

  let Instr{31-25} = op{9-3};
  let Instr{24-20} = k{21-17};
  let Instr{19-17} = op{2-0};
  let Instr{16-0}  = k{16-0};
}

//===----------------------------------------------------------------------===/
// Data Indirect with Displacement (X or Y + off) | op | Rr | q  |
//===----------------------------------------------------------------------===/
class DataIndirDisp<bits<5> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<5> Rr;
  bits<6> q;

  let Instr{15-14} = op{4-3};
  let Instr{13}    = q{5};
  let Instr{12}    = op{2};
  let Instr{11-10} = q{4-3};
  let Instr{9}     = op{1};
  let Instr{8-4}   = Rr;
  let Instr{3}     = op{0};
  let Instr{2-0}   = q{2-0};
}

//===----------------------------------------------------------------------===/
// Relative Program Adressing, RJMP and RCALL     | op |     k    |
//===----------------------------------------------------------------------===/
class RelProgAddr<bits<4> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<12> k;

  let Instr{15-12} = op;
  let Instr{11-0}  = k;
}

//===----------------------------------------------------------------------===/
// SREG bit manipulation (bclr, bset)             |     op      |s|
//===----------------------------------------------------------------------===/
class SREGBitMan<bits<13> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<3> s;

  let Instr{15-7} = op{12-4};
  let Instr{6-4}  = s;
  let Instr{3-0}  = op{3-0};
}

//===----------------------------------------------------------------------===/
// IO Reg bit manipulation (IO Reg < 32)          |   op   |  a |b|
//===----------------------------------------------------------------------===/
class IORegBitMan<bits<8> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<5> a;
  bits<3> b;

  let Instr{15-8} = op;
  let Instr{7-3}  = a;
  let Instr{2-0}  = b;
}

//===----------------------------------------------------------------------===/
// 8-bit operation with Imm                       | op| Rd|   K   |
//===----------------------------------------------------------------------===/
class OpImm<bits<4> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<4> Rd;
  bits<8> K;

  let Instr{15-12} = op;
  let Instr{11-8}  = Rd;
  let Instr{7-0}  = K;
}

//===----------------------------------------------------------------------===/
// 16-bit operation with Imm (ADIW, SBIW)         | op      |w| K |
//===----------------------------------------------------------------------===/
class WOpImm<bits<8> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<2> w;
  bits<6> K;

  let Instr{15-8} = op;
  let Instr{7-6}  = w;
  let Instr{5-0}  = K;
}

//===----------------------------------------------------------------------===/
// Conditional Branch                             | op |   k    |s|
//===----------------------------------------------------------------------===/
class CondBr<bits<6> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<7> k;
  bits<3> c;

  let Instr{15-10} = op;
  let Instr{9-3}  = k;
  let Instr{2-0}  = c;
}
/*
//===----------------------------------------------------------------------===/
// Relative Program Addressing (RJMP, RCALL)      | op|     k     |
//===----------------------------------------------------------------------===/
class CondBr<bits<4> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern>
        :WordInst<outs, ins, asmstr, pattern> 
{
  bits<12> k;
  
  let Instr{15-12} = op;
  let Instr{11-0}  = k;
}
*/
