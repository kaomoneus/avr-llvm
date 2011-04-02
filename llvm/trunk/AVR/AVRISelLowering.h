//==-------- AVRISelLowering.h - AVR DAG Lowering Interface ------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that AVR uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef __INCLUDE_AVRISELOWERING_H__
#define __INCLUDE_AVRISELOWERING_H__

#include "AVR.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm
{

namespace AVRISD
{
  // AVR Specific DAG Nodes
  enum NodeType
  {
    // Start the numbering where the builtin ops leave off.
    FIRST_NUMBER = ISD::BUILTIN_OP_END,
    RET_FLAG,
    LSL,
    LSR,
    ROL,
    ROR,
    ASR,
    CALL,
    Wrapper
  };
}

class CCState;

class AVRTargetLowering : public TargetLowering
{
public:
  explicit AVRTargetLowering(TargetMachine &TM);
public : // TargetLowering
  virtual MVT getShiftAmountTy(EVT LHSTy) const { return MVT::i8; }
  unsigned getFunctionAlignment(const Function *) const;
  const char *getTargetNodeName(unsigned Opcode) const;
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const;
private:
  void AnalyzeArguments(const Function *F, CCState &CCInfo) const;
  void AnalyzeReturn(CCState &CCInfo,
                     const SmallVectorImpl<ISD::OutputArg> &Outs) const;
  void AnalyzeCallResult(CCState &CCInfo,
                         const SmallVectorImpl<ISD::InputArg> &Ins) const;
  SDValue LowerCall(SDValue Chain,
                    SDValue Callee,
                    CallingConv::ID CallConv,
                    bool isVarArg,
                    bool &isTailCall,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals,
                    const SmallVectorImpl<ISD::InputArg> &Ins,
                    DebugLoc dl,
                    SelectionDAG &DAG,
                    SmallVectorImpl<SDValue> &InVals) const;
  SDValue LowerCallResult(SDValue Chain,
                          SDValue InFlag,
                          CallingConv::ID CallConv,
                          bool isVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins,
                          DebugLoc dl,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;
  SDValue LowerReturn(SDValue Chain,
                      CallingConv::ID CallConv,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals,
                      DebugLoc dl,
                      SelectionDAG &DAG) const;
  SDValue LowerFormalArguments(SDValue Chain,
                               CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               DebugLoc dl,
                               SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const;
  // Custom lowering functions
  SDValue LowerShifts(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
private:
  const TargetData *TD;
};

} // end namespace llvm

#endif // __INCLUDE_AVRISELOWERING_H__
