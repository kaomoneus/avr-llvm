//===------- AVRISelLowering.cpp - AVR DAG Lowering Implementation --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that AVR uses to lower LLVM code
// into a selection DAG.
//
//===----------------------------------------------------------------------===//

#include "AVRISelLowering.h"
#include "AVRTargetMachine.h"
#include "llvm/Function.h"
#include "llvm/Type.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

AVRTargetLowering::AVRTargetLowering(TargetMachine &TM) :
  TargetLowering(TM, new TargetLoweringObjectFileELF())
{
  TD = getTargetData();

  // Set up the register classes.
  addRegisterClass(MVT::i8, AVR::GPR8RegisterClass);
  //addRegisterClass(MVT::i16, AVR::WDREGSRegisterClass);
  //addRegisterClass(MVT::i32, AVR::DWREGSRegisterClass);
  //addRegisterClass(MVT::i64, AVR::QWREGSRegisterClass);

  addRegisterClass(MVT::i8, AVR::LDREGSRegisterClass);

  // Compute derived properties from the register classes
  computeRegisterProperties();

  //:TODO: check rest of config methods
  // Target configuration
  setBooleanContents(ZeroOrOneBooleanContent);
  setSchedulingPreference(Sched::RegPressure);
  setIntDivIsCheap(false);

  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);

  setOperationAction(ISD::SHL, MVT::i8, Custom);
  //setOperationAction(ISD::SHL, MVT::i16, Custom); //:TODO: TEST
  //setOperationAction(ISD::SRL, MVT::i16, Custom); //:TODO: TEST
  /*setOperationAction(ISD::ADD, MVT::i8, Legal); //:TODO: TEST
  setOperationAction(ISD::ADDE, MVT::i8, Legal); //:TODO: TEST
  setOperationAction(ISD::ADDC, MVT::i8, Legal); //:TODO: TEST
  setOperationAction(ISD::ADD, MVT::i16, Custom); //:TODO: TEST
  setOperationAction(ISD::ADDE, MVT::i16, Custom); //:TODO: TEST
  setOperationAction(ISD::ADDC, MVT::i16, Custom); //:TODO: TEST*/
  //setOperationAction(ISD::ADD, MVT::i16, Custom); //:TODO: TEST
  //setOperationAction(ISD::ADDE, MVT::i16, Expand); //:TODO: TEST
  //setOperationAction(ISD::ADDC, MVT::i16, Expand); //:TODO: TEST
  setOperationAction(ISD::SRL, MVT::i8, Custom);
  setOperationAction(ISD::SRA, MVT::i8, Custom);
  setOperationAction(ISD::ROTR, MVT::i8, Custom);
  setOperationAction(ISD::ROTL, MVT::i8, Custom);

  //setOperationAction(ISD::GlobalAddress, MVT::i16, Expand);

setOperationAction(ISD::SMUL_LOHI,        MVT::i8,    Legal);
setOperationAction(ISD::UMUL_LOHI,        MVT::i8,    Legal);
setOperationAction(ISD::MUL,              MVT::i16,   Expand);
setOperationAction(ISD::MUL,              MVT::i8,   Expand);
setOperationAction(ISD::MULHS,            MVT::i16,   Expand);
setOperationAction(ISD::MULHU,            MVT::i16,   Expand);
setOperationAction(ISD::SMUL_LOHI,        MVT::i16,   Expand);
setOperationAction(ISD::UMUL_LOHI,        MVT::i16,   Expand);
setOperationAction(ISD::SMUL_LOHI,        MVT::i32,   Expand);
setOperationAction(ISD::UMUL_LOHI,        MVT::i32,   Expand);
/*
setOperationAction(ISD::SHL_PARTS,        MVT::i8,    Expand);
setOperationAction(ISD::SHL_PARTS,        MVT::i16,   Expand);
setOperationAction(ISD::SRL_PARTS,        MVT::i8,    Expand);
setOperationAction(ISD::SRL_PARTS,        MVT::i16,   Expand);
setOperationAction(ISD::SRA_PARTS,        MVT::i8,    Expand);
setOperationAction(ISD::SRA_PARTS,        MVT::i16,   Expand);
  setOperationAction(ISD::CTTZ,             MVT::i8,    Expand);
setOperationAction(ISD::CTTZ,             MVT::i16,   Expand);
setOperationAction(ISD::CTLZ,             MVT::i8,    Expand);
setOperationAction(ISD::CTLZ,             MVT::i16,   Expand);
setOperationAction(ISD::CTPOP,            MVT::i8,    Expand);
setOperationAction(ISD::CTPOP,            MVT::i16,   Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);

  setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);
setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
setLoadExtAction(ISD::SEXTLOAD, MVT::i8,  Expand);
setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Expand);

    setOperationAction(ISD::SETCC, MVT::i8, Expand);
    setOperationAction(ISD::SETCC, MVT::i16, Expand);
    setOperationAction(ISD::SELECT, MVT::i8, Expand);
    setOperationAction(ISD::SELECT, MVT::i16, Expand);

setTruncStoreAction(MVT::i16, MVT::i8, Expand);

setOperationAction(ISD::ZERO_EXTEND, MVT::i16, Expand);
setOperationAction(ISD::SIGN_EXTEND, MVT::i16, Expand);*/
}

unsigned AVRTargetLowering::getFunctionAlignment(const Function *) const
{
  return 2; //:TODO: is this correct?
}

//:TODO: all the following expansion functions were my failed attempts to
// custom lower the add16 operation, may be removed if they're not needed
// as code ref.
SDValue ExpandOR(SDNode *N, SelectionDAG &DAG)
{
  DebugLoc dl = N->getDebugLoc();
    /*SDValue LHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(0, MVT::i8));
    SDValue LHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(1, MVT::i8));
    SDValue RHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(0, MVT::i8));
    SDValue RHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(1, MVT::i8));*/

    SDValue LHSL= DAG.getTargetExtractSubreg(0, dl, MVT::i16, N->getOperand(0));
    SDValue RHSL= DAG.getTargetExtractSubreg(1, dl, MVT::i16, N->getOperand(0));
    SDValue LHSH= DAG.getTargetExtractSubreg(0, dl, MVT::i16, N->getOperand(1));
    SDValue RHSH= DAG.getTargetExtractSubreg(1, dl, MVT::i16, N->getOperand(1));
    
    SDValue myor1 = DAG.getNode(ISD::OR, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSL, RHSL);
    SDValue myor2 = DAG.getNode(ISD::OR, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSH, RHSH);
    SDValue Lo(myor1.getNode(), 0);
    SDValue Hi(myor2.getNode(), 0);

   SDValue ops[2] = { Lo, Hi };
  return DAG.getMergeValues(ops, 2, dl);
      //return DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i16, Hi, Lo);
      //^THIS will loop forever
}

SDValue ExpandZEXT(SDNode *N, SelectionDAG &DAG)
{
  DebugLoc dl = N->getDebugLoc();
    SDValue LHSL= DAG.getTargetExtractSubreg(0, dl, MVT::i16, N->getOperand(0));
    SDValue LHSH= DAG.getTargetExtractSubreg(1, dl, MVT::i16, N->getOperand(0));
   SDValue myxor = DAG.getNode(ISD::XOR, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSH, LHSH);
    SDValue myor = DAG.getNode(ISD::AND, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSL, LHSL);
  SDValue Lo(myxor.getNode(), 0);
  SDValue Hi(myor.getNode(), 0);
  SDValue ops[2] = { Lo, Hi };
  return DAG.getMergeValues(ops, 2, dl);
    //return SDValue(myxor.getNode(), 0);
    //SDValue Lo(LHSL.getNode(), 0);
   //return DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i16, Hi, Lo);
}

SDValue ExpandADDSUB(SDNode *N, SelectionDAG &DAG)
{
  DebugLoc dl = N->getDebugLoc();
  // Extract components
  /*  SDValue LHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(0, MVT::i8));
    SDValue LHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(1, MVT::i8));
    SDValue RHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(0, MVT::i8));
    SDValue RHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(1, MVT::i8));
    SDValue myadd = DAG.getNode(ISD::ADD, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSL, RHSL);
    //SDValue Lo(myadd.getNode(), 0);
    SDValue carry(LHSL.getNode(), 1);
   SDValue myadd1 = DAG.getNode(ISD::ADD, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LHSH, RHSH);
   SDValue myadd2 = DAG.getNode(ISD::ADD, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  myadd, myadd1);
    //SDValue Hi(LHSH.getNode(), 0);
    
    return DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i16, myadd1, myadd2);
    

    //SDValue ops[3] = { LHSL, LHSH};
    //return DAG.getMergeValues(ops, 2, dl);

    SDValue LO1= DAG.getTargetExtractSubreg(0, dl, MVT::i16, N->getOperand(0));
    SDValue HI1= DAG.getTargetExtractSubreg(1, dl, MVT::i16, N->getOperand(0));
    SDValue LO2= DAG.getTargetExtractSubreg(0, dl, MVT::i16, N->getOperand(1));
    SDValue HI2= DAG.getTargetExtractSubreg(1, dl, MVT::i16, N->getOperand(1));
        SDValue myadd = DAG.getNode(ISD::ADDC, dl, DAG.getVTList(MVT::i8, MVT::i8),
                                  LO1, LO2);
        SDValue Lo(myadd.getNode(), 0);
    SDValue carry(myadd.getNode(), 1);
    SDValue myadde = DAG.getNode(ISD::ADDE, dl, DAG.getVTList(MVT::i8, MVT::i8, MVT::Other),
                                  HI1, HI2, carry);
    SDValue Hi(myadde.getNode(), 0);

    return DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i16, Hi, Lo);*/

    //SDValue ops[2] = { Lo, Hi};
    //return DAG.getMergeValues(ops, 2, dl);
/*
   SDValue LHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(0, MVT::i8));
    SDValue LHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(0),  DAG.getConstant(1, MVT::i8));
    SDValue RHSL = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(0, MVT::i8));
    SDValue RHSH = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16,
                              N->getOperand(1), DAG.getConstant(1, MVT::i8));*/

    /*SDNode *Sum = DAG.getMachineNode(AVR::ADDRdRr, dl, MVT::i8, MVT::i8,
                                   LHSL, RHSL);
    SDNode *Sum1 = DAG.getMachineNode(AVR::ADCRdRr, dl, MVT::i8, MVT::i8,
                                   LHSH, RHSH);
    SDValue ops[2] = { SDValue(Sum, 0), SDValue(Sum1, 0) };
    return DAG.getMergeValues(ops, 2, dl);*/
    return SDValue();
}

const char *AVRTargetLowering::getTargetNodeName(unsigned Opcode) const
{
  switch (Opcode)
  {
  case AVRISD::RET_FLAG:              return "AVRISD::RET_FLAG";
  case AVRISD::LSL:                   return "AVRISD::LSL";
  case AVRISD::LSR:                   return "AVRISD::LSR";
  case AVRISD::ROL:                   return "AVRISD::ROL";
  case AVRISD::ROR:                   return "AVRISD::ROR";
  case AVRISD::ASR:                   return "AVRISD::ASR";
  case AVRISD::CALL:                  return "AVRISD::CALL";
  case AVRISD::Wrapper:               return "AVRISD::Wrapper";
  default:                            return NULL;
  }
}

SDValue AVRTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode())
  {
  case ISD::SRA:
  case ISD::ROTL:
  case ISD::ROTR:
  case ISD::SRL:
  case ISD::SHL:                        return LowerShifts(Op, DAG);
  case ISD::GlobalAddress:              return LowerGlobalAddress(Op, DAG);
  case ISD::ADD:                        return ExpandADDSUB(Op.getNode(), DAG);
  case ISD::ADDE:                       return ExpandADDSUB(Op.getNode(), DAG);
  case ISD::OR:                         return ExpandOR(Op.getNode(), DAG);
  case ISD::ZERO_EXTEND:                return ExpandZEXT(Op.getNode(), DAG);
  default:
    llvm_unreachable("unimplemented operand");
    return SDValue();
  }
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//
#include "AVRGenCallingConv.inc"

//:TODO:
void
AVRTargetLowering::AnalyzeArguments(const Function *F, CCState &CCInfo) const
{
  static const unsigned ArgRegs[] =
  {
    AVR::R25, AVR::R24, AVR::R23, AVR::R22, AVR::R21, AVR::R20, AVR::R19,
    AVR::R18, AVR::R17, AVR::R16, AVR::R15, AVR::R14, AVR::R13, AVR::R12,
    AVR::R11, AVR::R10, AVR::R9, AVR::R8
  };

  unsigned RegNum = array_lengthof(ArgRegs);
  unsigned RegsUsed = 0;
  unsigned ArgNo = 0;

  for (Function::const_arg_iterator I = F->arg_begin(), E = F->arg_end();
       I != E;
       ++I)
  {
    const Type *Ty = I->getType();
    uint64_t size = TD->getTypeSizeInBits(Ty) / 8;

    if (size > (RegNum - RegsUsed))
    {
      assert(0 && "Implement AnalyzeArguments stack handling");
      break; //:TODO: allocate stack slots here
    }

    if (size == 1)
    {
      if (!(RegsUsed & 1))
      {
        ++RegsUsed;
      }
      unsigned Reg = CCInfo.AllocateReg(ArgRegs[RegsUsed++]);
      CCInfo.addLoc(CCValAssign::getCustomReg(ArgNo++, MVT::i8, Reg, MVT::i8,
                                              CCValAssign::Full));
    }
    else
    {
      for (unsigned i = 0; i != size; ++i)
      {
        unsigned Reg = CCInfo.AllocateReg(ArgRegs[RegsUsed++]);
        CCInfo.addLoc(CCValAssign::getCustomReg(ArgNo++, MVT::i8, Reg, MVT::i8,
                                                CCValAssign::Full));
      }
    }
  }
}

void
AVRTargetLowering::AnalyzeReturn(CCState &CCInfo,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs)
                                 const
{
  if (Outs.size() == 1)
  {
    // If we are only returning a byte then assign it to R24.
    unsigned Reg = CCInfo.AllocateReg(AVR::R24);
    CCInfo.addLoc(CCValAssign::getCustomReg(0, MVT::i8, Reg, MVT::i8,
                                            CCValAssign::Full));
  }
  else
  {
    // Analize return values.
    CCInfo.AnalyzeReturn(Outs, RetCC_AVR);
  }
}

void
AVRTargetLowering::AnalyzeCallResult(CCState &CCInfo,
                                     const SmallVectorImpl<ISD::InputArg> &Ins)
                                     const
{
  if (Ins.size() == 1)
  {
    // If we are only returning a byte then assign it to R24.
    unsigned Reg = CCInfo.AllocateReg(AVR::R24);
    CCInfo.addLoc(CCValAssign::getCustomReg(0, MVT::i8, Reg, MVT::i8,
                                            CCValAssign::Full));
  }
  else
  {
    // Analize return values.
    CCInfo.AnalyzeCallResult(Ins, RetCC_AVR);
  }
}

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "llvm/ValueSymbolTable.h"
SDValue
AVRTargetLowering::LowerCall(SDValue Chain, SDValue Callee,
                             CallingConv::ID CallConv, bool isVarArg,
                             bool &isTailCall,
                             const SmallVectorImpl<ISD::OutputArg> &Outs,
                             const SmallVectorImpl<SDValue> &OutVals,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const
{
  //:TODO: IMPLEMENT VARARGS!
  // AVR does not yet support tail call optimization
  isTailCall = false;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		  DAG.getTarget(), ArgLocs, *DAG.getContext());

  SmallVector<unsigned, 8> ArgSizes;

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
  {
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i8); //:TODO: HACK HACK!!! this is i16!!
    const Function *F = cast<Function>(G->getGlobal());
    AnalyzeArguments(F, CCInfo);
  }
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
  {
    assert(0 && "Implement ExternalSymbol lower call");
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);
  }

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();
  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true));

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i)
  {
    CCValAssign &VA = ArgLocs[i];
    EVT RegVT = VA.getLocVT();
    SDValue Arg = OutVals[i];

    // Promote the value if needed.
    switch (VA.getLocInfo())
    {
    default:
      llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, RegVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, RegVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, RegVT, Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc())
    {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    }
    else
    {
      // Register can't get to this point...
      assert(VA.isMemLoc());
      assert(0 && "Stack is not supported yet");
    }
  }

  // Transform all store nodes into one single node because all store
  // nodes are independent of each other.
  if (!MemOpChains.empty())
  {
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, &MemOpChains[0],
                        MemOpChains.size());
  }

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emited instructions must be
  // stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
  {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Other);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
  {
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));
  }

  if (InFlag.getNode())
  {
    Ops.push_back(InFlag);
  }

  Chain  = DAG.getNode(AVRISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getIntPtrConstant(NumBytes, true),
                             DAG.getIntPtrConstant(0, true), InFlag);
  if (!Ins.empty())
  {
    InFlag = Chain.getValue(1);
  }

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, dl, DAG,
                         InVals);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue
AVRTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                   CallingConv::ID CallConv, bool isVarArg,
                                   const SmallVectorImpl<ISD::InputArg> &Ins,
                                   DebugLoc dl, SelectionDAG &DAG,
                                   SmallVectorImpl<SDValue> &InVals) const
{
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		  DAG.getTarget(), RVLocs, *DAG.getContext());

  AnalyzeCallResult(CCInfo, Ins);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i)
  {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//
SDValue
AVRTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               DebugLoc dl, SelectionDAG &DAG) const
{
  //:TODO: how are structs returned here?
  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		  DAG.getTarget(), RVLocs, *DAG.getContext());

  AnalyzeReturn(CCInfo, Outs);

  // If this is the first return lowered for this function, add the regs to
  // the liveout set for the function.
  MachineRegisterInfo &MRI = DAG.getMachineFunction().getRegInfo();
  unsigned e = RVLocs.size();

  if (MRI.liveout_empty())
  {
    for (unsigned i = 0; i != e; ++i)
    {
      if (RVLocs[i].isRegLoc())
      {
        MRI.addLiveOut(RVLocs[i].getLocReg());
      }
    }
  }

  SDValue Flag;
  // Copy the result values into the output registers.
  for (unsigned i = 0; i != e; ++i)
  {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
  }

  if (Flag.getNode())
  {
    return DAG.getNode(AVRISD::RET_FLAG, dl, MVT::Other, Chain, Flag);
  }

  return DAG.getNode(AVRISD::RET_FLAG, dl, MVT::Other, Chain);
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
AVRTargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                        bool isVarArg,
                                        const SmallVectorImpl
                                        <ISD::InputArg> &Ins, DebugLoc dl,
                                        SelectionDAG &DAG,
                                        SmallVectorImpl<SDValue> &InVals) const
{
  MachineFunction &MF = DAG.getMachineFunction();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		  DAG.getTarget(), ArgLocs, *DAG.getContext());

  unsigned ArgRegEnd;
  unsigned LastVal = ~0U;

  const Function *F = MF.getFunction();
  AnalyzeArguments(F, CCInfo);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i)
  {
    CCValAssign &VA = ArgLocs[i];
    assert((VA.getValNo() != LastVal)
           && "Don't support value assigned to multiple locs yet");
    LastVal = VA.getValNo();

    // Arguments stored on registers
    if (VA.isRegLoc())
    {
      EVT RegVT = VA.getLocVT();
      TargetRegisterClass *RC = NULL;

      switch (RegVT.getSimpleVT().SimpleTy)
      {
      default:
        llvm_unreachable("LowerFormalArguments Unhandled argument type:");
      case MVT::i8:
        RC = AVR::GPR8RegisterClass;
        break;
      case MVT::i16:
        //RC = AVR::WDREGSRegisterClass;
        break;
      case MVT::i32:
        //RC = AVR::DWREGSRegisterClass;
        break;
      case MVT::i64:
        //RC = AVR::QWREGSRegisterClass;
        break;
      }
      // Transform the arguments stored on
      // physical registers into virtual ones
RC = AVR::GPR8RegisterClass;
      unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);

      if (VA.getLocInfo() == CCValAssign::SExt)
      {
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
      }
      else if (VA.getLocInfo() == CCValAssign::ZExt)
      {
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
      }

      if (VA.getLocInfo() != CCValAssign::Full)
      {
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
      }

      InVals.push_back(ArgValue);
    }
    else
    {
      // sanity check
      assert(VA.isMemLoc());
      assert(0 && "FormalARg lowering in memory isnt implemented yet");

      // The last argument is not a register anymore
      ArgRegEnd = 0;

      // The stack pointer offset is relative to the caller stack frame.
      // Since the real stack size is unknown here, a negative SPOffset
      // is used so there's a way to adjust these offsets when the stack
      // size get known (on EliminateFrameIndex). A dummy SPOffset is
      // used instead of a direct negative address (which is recorded to
      // be used on emitPrologue) to avoid mis-calc of the first stack
      // offset on PEI::calculateFrameObjectOffsets.
      // Arguments are always 32-bit.
      /* unsigned ArgSize = VA.getLocVT().getSizeInBits()/8;
      int FI = MFI->CreateFixedObject(ArgSize, 0, true, false);
      //MipsFI->recordLoadArgsFI(FI, -(ArgSize+
        // (FirstStackArgLoc + VA.getLocMemOffset())));

      // Create load nodes to retrieve arguments from the stack
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
      InVals.push_back(DAG.getLoad(VA.getValVT(), dl, Chain, FIN, NULL, 0,
                                  false, false, 0));*/
    }
  }

    // All stores are grouped in one node to allow the matching between
    // the size of Ins and InVals. This only happens when on varg functions
    /*if (!OutChains.empty())
    {
        OutChains.push_back(Chain);
        Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, &OutChains[0],
                            OutChains.size());
    }*/

  return Chain;
}

SDValue AVRTargetLowering::LowerShifts(SDValue Op, SelectionDAG &DAG) const
{
  //:TODO: this function has to be completely rewritten to produce optimal
  // code, atm it's producing long but correct code.
  unsigned Opc = Op.getOpcode();
  unsigned Opc2;
  SDNode* N = Op.getNode();
  EVT VT = Op.getValueType();
  DebugLoc dl = N->getDebugLoc();
  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();

  // Expand the stuff into sequence of shifts.
  // FIXME: for some shift amounts this might be done better!
  // E.g.: foo >> (8 + N) => sxt(swpb(foo)) >> N
  SDValue Victim = N->getOperand(0);

  /* if (Opc == ISD::SRL && ShiftAmount) {
      // Emit a special goodness here:
      // srl A, 1 => clrc; rrc A
      Victim = DAG.getNode(MSP430ISD::RRC, dl, VT, Victim);
      ShiftAmount -= 1;
  }*/
  switch (Opc)
  {
  case ISD::SRA:
    Opc2 = AVRISD::ASR;
    break;
  case ISD::ROTL:
    Opc2 = AVRISD::ROL;
    break;
  case ISD::ROTR:
    Opc2 = AVRISD::ROR;
    break;
  case ISD::SRL:
    Opc2 = AVRISD::LSR;
    break;
  case ISD::SHL:
    Opc2 = AVRISD::LSL;
    break;
  default:
    llvm_unreachable("Invalid shift opcode");
  }

  while (ShiftAmount--)
  {
    Victim = DAG.getNode(Opc2, dl, MVT::i8, Victim);
  }

  return Victim;
}

SDValue
AVRTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  assert(0 && "custom GLOBADDRESD" );
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();

  Op = DAG.getTargetGlobalAddress(GV, dl, getPointerTy());
  return DAG.getNode(AVRISD::Wrapper, dl, getPointerTy(), Op);
}
