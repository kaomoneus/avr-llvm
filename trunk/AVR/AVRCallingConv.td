//===- AVRCallingConv.td - Calling Conventions AVR 32/64 ---*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the AVR. The CC aims to be 
// compatible to avr-libc. http://www.nongnu.org/avr-libc/user-manual/FAQ.html
//
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

// Return-value convention.
def RetCC_AVR : CallingConv<[
  // Promote i8 arguments to i16.
  //CCIfType<[i8], CCPromoteToType<i16>>,

  CCIfType<[i8] , CCAssignToReg<[R24]>>,
  CCIfType<[i16], CCAssignToReg<[R25R24]>>,
  CCIfType<[i32], CCAssignToReg<[R25R24, R23R22]>>,
  CCIfType<[i64], CCAssignToReg<[R25R24, R23R22, R21R20, R19R18]>>
]>;


//===----------------------------------------------------------------------===//
// AVR Argument Calling Convention
//===----------------------------------------------------------------------===//

def CC_AVR : CallingConv<[
  
  // If the current function dosen't take variable arguments
  //TODO: add CCIfNotVarArg, if required
  //CCIfNotVarArg<
  
  // the first arguments are passed in GPRs and
  CCIfType<[i8], CCAssignToReg<[R24, R22, R20, R18, R16, R14, R12, R10, R8]>>,
  CCIfType<[i16], CCAssignToReg<[R25R24, R23R22, R21R20, R19R18, R17R16,
                                 R15R14, R13R12, R11R10, R9R8]>>,
  CCIfType<[i32], CCAssignToReg<[R25R22, R21R18, R17R14, R13R10]>>,
  CCIfType<[i64], CCAssignToReg<[R25R18, R17R10]>>,
  
  // all other arguments on the stack
  //TODO: fix AssignToStack arguments
  //CCIfType<[i8,i16,i32,i64], CCAssignToStack<[1, 1]>>,
  
  // arguments to functions with variable argument count are passed on the 
  // stack. i8 is extended to i16.
  CCIfType<[i16,i32,i64], CCAssignToStack<1, 1>>
]>;

