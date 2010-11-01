//=------ AVRCallingConv.td - Calling Conventions for AVR ----*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for the AVR architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AVR Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_AVR : CallingConv
<[
  //:NOTE: This code is complemented with customized C++ code in
  // AVRISelLowering.cpp to store a char sized return value in R24

  // i8 is zero/sign extended to i16
  //:TODO: r25 should be sign/zero extended, check out what gcc does
  // CCIfType<[i8], CCBitConvertToType<i16>>,
  CCIfType<[i8], CCAssignToReg<[R25, R24, R23, R22, R21, R20, R19, R18]>>

  //:TODO: all this code was added when i thought of using register pairs
  // but LLVM cant handle them, leave this for a while until we're sure
  // it's not going to be needed
  // i16 are returned in registers R25:R24
  //CCIfType<[i16], CCAssignToReg<[R25R24]>>

  // i32 are returned in registers R25:R24:R23:R22
  //CCIfType<[i32, f32], CCAssignToReg<[R25R22]>>,
  //CCIfType<[i32, f32], CCAssignToReg<[R25R24, R23R22]>>

  // i64 are returned in registers R25:R24:R23:R22:R21:R20:R19:R18
  //CCIfType<[i64, f64], CCAssignToReg<[R25R18]>>
]>;

//===----------------------------------------------------------------------===//
// AVR Argument Calling Conventions
//===----------------------------------------------------------------------===//
class CCIfNotSplit<CCAction A> : CCIf<"!ArgFlags.isSplit()", A> {}

def CC_AVR : CallingConv
<[
  //:NOTE: This code is complemented with customized C++ code in
  // AVRISelLowering.cpp to put char sized arguments in even registers

  //:TODO: handle varargs!
  // i8 are passed in even registers
  //CCIfType<[i8], CCPromoteToType<i16>>,
  //CCIfType<[i8], CCAssignToReg<[R24, R22, R20, R18, R16, R14, R12, R10, R8]>>,

  //CCIfType<[i16], CCAssignToReg<[R25R24, R23R22, R21R20, R19R18,
  //                               R17R16, R15R14, R13R12, R11R10, R9R8]>>
  //CCIfType<[i16, i8], CCAssignToReg<[R25,R24, R23,R22, R21,R20, R19,R18,
    //                           R17,R16, R15,R14, R13,R12, R11,R10, R9,R8]>>
  //CCCustom<"CC_AVR_Custom_AlignArgRegs">,
  //CCIfType<[i8], CCIfNotSplit<CCAssignToReg<[R24, R22, R20, R18,>>
//CCIfSplit<CCAssignToReg<[R24, R22, R20, R18,
                    //           R16, R14, R12,R10, R8]>>,
  CCIfType<[i8], CCAssignToReg<[R25,R24, R23,R22, R21,R20, R19,R18,
                                R17,R16, R15,R14, R13,R12, R11,R10, R9,R8]>>
    //                           R17,R16, R15,R14, R13,R12, R11,R10, R9,R8]>>
  //CCIfType<[i32], CCAssignToReg<[R25R22, R21R18, R17R14, R13R10]>>,

  //CCIfType<[i64], CCAssignToReg<[R25R18, R17R10]>>,
  //:TODO: fix these args
  //CCAssignToStack<0, 0>
]>;
