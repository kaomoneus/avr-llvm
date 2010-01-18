//===-- AVRISelLowering.cpp - AVR DAG Lowering Implementation  ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the AVRTargetLowering class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "avr"

#include "AVRISelLowering.h"
#include "AVR.h"
#include "AVRTargetMachine.h"
#include "AVRSubtarget.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Intrinsics.h"
#include "llvm/CallingConv.h"
#include "llvm/GlobalVariable.h"
#include "llvm/GlobalAlias.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/VectorExtras.h"
using namespace llvm;

AVRTargetLowering::AVRTargetLowering(AVRTargetMachine &tm) :
  TargetLowering(tm, new TargetLoweringObjectFileELF()),
  ST(*tm.getSubtargetImpl()), TM(tm)
{

  // Set up the register classes.
  addRegisterClass(MVT::i8,  AVR::GPRegsRegisterClass);
  addRegisterClass(MVT::i16, AVR::ADIWRegsRegisterClass);
  addRegisterClass(MVT::i8,  AVR::LDIRegsRegisterClass);
  addRegisterClass(MVT::i8, AVR::NoLDIGPRegsRegisterClass);
  addRegisterClass(MVT::i8,  AVR::FMULRegsRegisterClass);
  addRegisterClass(MVT::i16, AVR::WRegsRegisterClass);
  addRegisterClass(MVT::i32,  AVR::DWRegsRegisterClass);
  addRegisterClass(MVT::i64, AVR::QWRegsRegisterClass);
  addRegisterClass(MVT::i16,  AVR::PTRRegsRegisterClass);
  addRegisterClass(MVT::i16, AVR::BasePTRRegsRegisterClass);
  addRegisterClass(MVT::i16,  AVR::MulRetRegRegisterClass);
  addRegisterClass(MVT::i1, AVR::StatusFlagsRegisterClass);
  // Compute derived properties from the register classes
  computeRegisterProperties();

  // Provide all sorts of operation actions

  // Division is expensive
  setIntDivIsCheap(false);

  // Even if we have only 1 bit shift here, we can perform
  // shifts of the whole bitwidth 1 bit per step.
  setShiftAmountType(MVT::i8);

  setStackPointerRegisterToSaveRestore(AVR::SP);
  setBooleanContents(ZeroOrOneBooleanContent);
  setSchedulingPreference(SchedulingForLatency);

  setLoadExtAction(ISD::EXTLOAD,  MVT::i1, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8, Expand);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Expand);

  // We don't have any truncstores
  setTruncStoreAction(MVT::i16, MVT::i8, Expand);

  setOperationAction(ISD::SRA,              MVT::i8,    Custom);
  setOperationAction(ISD::SHL,              MVT::i8,    Custom);
  setOperationAction(ISD::SRL,              MVT::i8,    Custom);
  setOperationAction(ISD::SRA,              MVT::i16,   Custom);
  setOperationAction(ISD::SHL,              MVT::i16,   Custom);
  setOperationAction(ISD::SRL,              MVT::i16,   Custom);
  setOperationAction(ISD::ROTL,             MVT::i8,    Expand);
  setOperationAction(ISD::ROTR,             MVT::i8,    Expand);
  setOperationAction(ISD::ROTL,             MVT::i16,   Expand);
  setOperationAction(ISD::ROTR,             MVT::i16,   Expand);
  setOperationAction(ISD::GlobalAddress,    MVT::i16,   Custom);
  setOperationAction(ISD::ExternalSymbol,   MVT::i16,   Custom);
  setOperationAction(ISD::BR_JT,            MVT::Other, Expand);
  setOperationAction(ISD::BRIND,            MVT::Other, Expand);
  setOperationAction(ISD::BR_CC,            MVT::i8,    Custom);
  setOperationAction(ISD::BR_CC,            MVT::i16,   Custom);
  setOperationAction(ISD::BRCOND,           MVT::Other, Expand);
  setOperationAction(ISD::SETCC,            MVT::i8,    Expand);
  setOperationAction(ISD::SETCC,            MVT::i16,   Expand);
  setOperationAction(ISD::SELECT,           MVT::i8,    Expand);
  setOperationAction(ISD::SELECT,           MVT::i16,   Expand);
  setOperationAction(ISD::SELECT_CC,        MVT::i8,    Custom);
  setOperationAction(ISD::SELECT_CC,        MVT::i16,   Custom);
  setOperationAction(ISD::SIGN_EXTEND,      MVT::i16,   Custom);

  setOperationAction(ISD::CTTZ,             MVT::i8,    Expand);
  setOperationAction(ISD::CTTZ,             MVT::i16,   Expand);
  setOperationAction(ISD::CTLZ,             MVT::i8,    Expand);
  setOperationAction(ISD::CTLZ,             MVT::i16,   Expand);
  setOperationAction(ISD::CTPOP,            MVT::i8,    Expand);
  setOperationAction(ISD::CTPOP,            MVT::i16,   Expand);

  setOperationAction(ISD::SHL_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SHL_PARTS,        MVT::i16,   Expand);
  setOperationAction(ISD::SRL_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SRL_PARTS,        MVT::i16,   Expand);
  setOperationAction(ISD::SRA_PARTS,        MVT::i8,    Expand);
  setOperationAction(ISD::SRA_PARTS,        MVT::i16,   Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1,   Expand);

  // FIXME: Implement efficiently multiplication by a constant
  setOperationAction(ISD::MUL,              MVT::i16,   Expand);
  setOperationAction(ISD::MULHS,            MVT::i16,   Expand);
  setOperationAction(ISD::MULHU,            MVT::i16,   Expand);
  setOperationAction(ISD::SMUL_LOHI,        MVT::i16,   Expand);
  setOperationAction(ISD::UMUL_LOHI,        MVT::i16,   Expand);

  setOperationAction(ISD::UDIV,             MVT::i16,   Expand);
  setOperationAction(ISD::UDIVREM,          MVT::i16,   Expand);
  setOperationAction(ISD::UREM,             MVT::i16,   Expand);
  setOperationAction(ISD::SDIV,             MVT::i16,   Expand);
  setOperationAction(ISD::SDIVREM,          MVT::i16,   Expand);
  setOperationAction(ISD::SREM,             MVT::i16,   Expand);
}

SDValue AVRTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG)
{
  switch (Op.getOpcode())
  {
    case ISD::SHL: // FALLTHROUGH
    case ISD::SRL:
    case ISD::SRA:              return LowerShifts(Op, DAG);
    case ISD::GlobalAddress:    return LowerGlobalAddress(Op, DAG);
    case ISD::ExternalSymbol:   return LowerExternalSymbol(Op, DAG);
    case ISD::BR_CC:            return LowerBR_CC(Op, DAG);
    case ISD::SELECT_CC:        return LowerSELECT_CC(Op, DAG);
    case ISD::SIGN_EXTEND:      return LowerSIGN_EXTEND(Op, DAG);
    default:
      llvm_unreachable("unimplemented operand");
      return SDValue();
  }
}

/// getFunctionAlignment - Return the Log2 alignment of this function.
unsigned AVRTargetLowering::getFunctionAlignment(const Function *F) const {
  return F->hasFnAttr(Attribute::OptimizeForSize) ? 1 : 4;
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "AVRGenCallingConv.inc"

SDValue
AVRTargetLowering::LowerFormalArguments(SDValue Chain,
                         CallingConv::ID CallConv, bool isVarArg,
                         const SmallVectorImpl<ISD::InputArg> &Ins,
                         DebugLoc dl, SelectionDAG &DAG,
                         SmallVectorImpl<SDValue> &InVals)
{
  DEBUG(errs() << "CallingConvention: " << CallConv <<"\n");

  switch (CallConv)
  {
    default:
      llvm_unreachable("Unsupported calling convention");
    case CallingConv::C:
    case CallingConv::Fast:
      return LowerCCCArguments(Chain, CallConv, isVarArg, Ins, dl, DAG, InVals);
  }
  return Chain;
}

SDValue
AVRTargetLowering::LowerCall(SDValue Chain, SDValue Callee,
                                CallingConv::ID CallConv, bool isVarArg,
                                bool isTailCall,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<ISD::InputArg> &Ins,
                                DebugLoc dl, SelectionDAG &DAG,
                                SmallVectorImpl<SDValue> &InVals)
{
#if 0
  switch (CallConv) {
  default:
    llvm_unreachable("Unsupported calling convention");
  case CallingConv::Fast:
  case CallingConv::C:
    return LowerCCCCallTo(Chain, Callee, CallConv, isVarArg, isTailCall,
                          Outs, Ins, dl, DAG, InVals);
  }
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

/// LowerCCCArguments - transform physical registers into virtual registers and
/// generate load operations for arguments places on the stack.
// FIXME: struct return stuff
// FIXME: varargs
SDValue
AVRTargetLowering::LowerCCCArguments(SDValue Chain,
                                        CallingConv::ID CallConv,
                                        bool isVarArg,
                                        const SmallVectorImpl<ISD::InputArg>
                                          &Ins,
                                        DebugLoc dl,
                                        SelectionDAG &DAG,
                                        SmallVectorImpl<SDValue> &InVals) 
{


  MachineFunction &MF = DAG.getMachineFunction();
  //MachineFrameInfo *MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_AVR);
  // TODO varargs support
  assert(!isVarArg && "Varargs not supported yet");

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) 
  {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) 
    {

      DEBUG(errs() << "Argument in Register\n");
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();

      switch (RegVT.getSimpleVT().SimpleTy) 
      {
      default:
        {
          DEBUG(errs() << "LowerFormalArguments Unhandled argument type: "
               << RegVT.getSimpleVT().SimpleTy << "\n");
          DEBUG(errs() << "EVT string: " << RegVT.getEVTString() << "\n");
          llvm_unreachable("Argument type not supported yet");
          llvm_unreachable(0);
        }
      case MVT::i8:
        unsigned VReg =
        RegInfo.createVirtualRegister(AVR::GPRegsRegisterClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg);
        SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);

        // If this is an 8-bit value, it is really passed promoted to 16
        // bits. Insert an assert[sz]ext to capture this, then truncate to the
        // right size.
        if (VA.getLocInfo() == CCValAssign::SExt)
        {
          llvm_unreachable("SExt not supported yet");
          ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        }
        else if (VA.getLocInfo() == CCValAssign::ZExt)
        {
          llvm_unreachable("ZExt not supported yet");
          ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        }

        if (VA.getLocInfo() != CCValAssign::Full)
        {
          llvm_unreachable("Full not supported yet");
          ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        }

        InVals.push_back(ArgValue);
#if 0
      case MVT::i16:
        unsigned VReg =
        RegInfo.createVirtualRegister(AVR::GPRegsRegisterClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg);
        SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);

        // If this is an 8-bit value, it is really passed promoted to 16
        // bits. Insert an assert[sz]ext to capture this, then truncate to the
        // right size.
        if (VA.getLocInfo() == CCValAssign::SExt)
          ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));

        if (VA.getLocInfo() != CCValAssign::Full)
          ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);

        InVals.push_back(ArgValue);
#endif
      }
    } 
    else 
    {
      // Sanity check
      assert(VA.isMemLoc() && "not in Reg and not in Mem??");
      
      DEBUG(errs() << "Argument on Stack\n");

      llvm_unreachable("Arguments on stack are not implemented yet!");
#if 0
      // Load the argument to a virtual register
      unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
      if (ObjSize > 2) {
        cerr << "LowerFormalArguments Unhandled argument type: "
             << VA.getLocVT().getSimpleVT().SimpleTy
             << "\n";
      }
      // Create the frame index object for this incoming parameter...
      int FI = MFI->CreateFixedObject(ObjSize, VA.getLocMemOffset());

      // Create the SelectionDAG nodes corresponding to a load
      //from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i16);
      InVals.push_back(DAG.getLoad(VA.getLocVT(), dl, Chain, FIN,
                                   PseudoSourceValue::getFixedStack(FI), 0));
#endif
    }
  }

  return Chain;
}

SDValue
AVRTargetLowering::LowerReturn(SDValue Chain,
                                  CallingConv::ID CallConv, bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  DebugLoc dl, SelectionDAG &DAG) {
  DEBUG(errs() << "return outs: " << Outs.size() << "\n");

  assert(!isVarArg && "VarArgs are not supported!");
  //assert((Outs.size() == 0) && "Only void returns are supported yet!");


  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_AVR);

  // If this is the first return lowered for this function, add the regs to the
  // liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                             Outs[i].Val, Flag);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
  }

  if (Flag.getNode())
    return DAG.getNode(AVRISD::RET_FLAG, dl, MVT::Other, Chain, Flag);

  // Return Void
  return DAG.getNode(AVRISD::RET_FLAG, dl, MVT::Other, Chain);
}

/// LowerCCCCallTo - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
/// TODO: sret.
SDValue
AVRTargetLowering::LowerCCCCallTo(SDValue Chain, SDValue Callee,
                                     CallingConv::ID CallConv, bool isVarArg,
                                     bool isTailCall,
                                     const SmallVectorImpl<ISD::OutputArg>
                                       &Outs,
                                     const SmallVectorImpl<ISD::InputArg> &Ins,
                                     DebugLoc dl, SelectionDAG &DAG,
                                     SmallVectorImpl<SDValue> &InVals) {/*
  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_AVR);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain ,DAG.getConstant(NumBytes,
                                                      getPointerTy(), true));

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SmallVector<SDValue, 12> MemOpChains;
  SDValue StackPtr;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    SDValue Arg = Outs[i].Val;

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
        break;
    }

    // Arguments that can be passed on register must be kept at RegsToPass
    // vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());

      if (StackPtr.getNode() == 0)
        StackPtr = DAG.getCopyFromReg(Chain, dl, AVR::SP, getPointerTy());

      SDValue PtrOff = DAG.getNode(ISD::ADD, dl, getPointerTy(),
                                   StackPtr,
                                   DAG.getIntPtrConstant(VA.getLocMemOffset()));


      MemOpChains.push_back(DAG.getStore(Chain, dl, Arg, PtrOff,
                                         PseudoSourceValue::getStack(),
                                         VA.getLocMemOffset()));
    }
  }

  // Transform all store nodes into one single node because all store nodes are
  // independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emited instructions must be stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), MVT::i16);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Flag);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain = DAG.getNode(AVRISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getConstant(NumBytes, getPointerTy(), true),
                             DAG.getConstant(0, getPointerTy(), true),
                             InFlag);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, dl,
                         DAG, InVals);*/
  llvm_unreachable("Not implemented");
  return SDValue();
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue
AVRTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                      CallingConv::ID CallConv, bool isVarArg,
                                      const SmallVectorImpl<ISD::InputArg> &Ins,
                                      DebugLoc dl, SelectionDAG &DAG,
                                      SmallVectorImpl<SDValue> &InVals)
{
#if 0
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_AVR);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

SDValue AVRTargetLowering::LowerShifts(SDValue Op,
                                          SelectionDAG &DAG)
{
#if 0
  unsigned Opc = Op.getOpcode();
  SDNode* N = Op.getNode();
  EVT VT = Op.getValueType();
  DebugLoc dl = N->getDebugLoc();

  // We currently only lower shifts of constant argument.
  if (!isa<ConstantSDNode>(N->getOperand(1)))
    return SDValue();

  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();

  // Expand the stuff into sequence of shifts.
  // FIXME: for some shift amounts this might be done better!
  // E.g.: foo >> (8 + N) => sxt(swpb(foo)) >> N
  SDValue Victim = N->getOperand(0);

  if (Opc == ISD::SRL && ShiftAmount) {
    // Emit a special goodness here:
    // srl A, 1 => clrc; rrc A
    Victim = DAG.getNode(AVRISD::RRC, dl, VT, Victim);
    ShiftAmount -= 1;
  }

  while (ShiftAmount--)
    Victim = DAG.getNode((Opc == ISD::SHL ? AVRISD::RLA : AVRISD::RRA),
                         dl, VT, Victim);

  return Victim;
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

SDValue AVRTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG)
{
#if 0
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();

  // Create the TargetGlobalAddress node, folding in the constant offset.
  SDValue Result = DAG.getTargetGlobalAddress(GV, getPointerTy(), Offset);
  return DAG.getNode(AVRISD::Wrapper, Op.getDebugLoc(),
                     getPointerTy(), Result);
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

SDValue AVRTargetLowering::LowerExternalSymbol(SDValue Op,
                                                  SelectionDAG &DAG)
{
#if 0
  DebugLoc dl = Op.getDebugLoc();
  const char *Sym = cast<ExternalSymbolSDNode>(Op)->getSymbol();
  SDValue Result = DAG.getTargetExternalSymbol(Sym, getPointerTy());

  return DAG.getNode(AVRISD::Wrapper, dl, getPointerTy(), Result);
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

#if 0
static SDValue EmitCMP(SDValue &LHS, SDValue &RHS, unsigned &TargetCC,
                       ISD::CondCode CC,
                       DebugLoc dl, SelectionDAG &DAG) {
  // FIXME: Handle bittests someday
  assert(!LHS.getValueType().isFloatingPoint() && "We don't handle FP yet");

  // FIXME: Handle jump negative someday
  TargetCC = AVR::COND_INVALID;
  switch (CC) {
  default: llvm_unreachable("Invalid integer condition!");
  case ISD::SETEQ:
    TargetCC = AVR::COND_E;  // aka COND_Z
    break;
  case ISD::SETNE:
    TargetCC = AVR::COND_NE; // aka COND_NZ
    break;
  case ISD::SETULE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETUGE:
    TargetCC = AVR::COND_HS; // aka COND_C
    break;
  case ISD::SETUGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETULT:
    TargetCC = AVR::COND_LO; // aka COND_NC
    break;
  case ISD::SETLE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETGE:
    TargetCC = AVR::COND_GE;
    break;
  case ISD::SETGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETLT:
    TargetCC = AVR::COND_L;
    break;
  }

  return DAG.getNode(AVRISD::CMP, dl, MVT::Flag, LHS, RHS);
  assert(0 && "Not implemented");
  return SDValue();
}
#endif

SDValue AVRTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG)
{
#if 0
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS   = Op.getOperand(2);
  SDValue RHS   = Op.getOperand(3);
  SDValue Dest  = Op.getOperand(4);
  DebugLoc dl   = Op.getDebugLoc();

  unsigned TargetCC = AVR::COND_INVALID;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  return DAG.getNode(AVRISD::BR_CC, dl, Op.getValueType(),
                     Chain,
                     Dest, DAG.getConstant(TargetCC, MVT::i8),
                     Flag);
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

SDValue AVRTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) 
{
#if 0
  SDValue LHS    = Op.getOperand(0);
  SDValue RHS    = Op.getOperand(1);
  SDValue TrueV  = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  DebugLoc dl    = Op.getDebugLoc();

  unsigned TargetCC = AVR::COND_INVALID;
  SDValue Flag = EmitCMP(LHS, RHS, TargetCC, CC, dl, DAG);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Flag);
  SmallVector<SDValue, 4> Ops;
  Ops.push_back(TrueV);
  Ops.push_back(FalseV);
  Ops.push_back(DAG.getConstant(TargetCC, MVT::i8));
  Ops.push_back(Flag);

  return DAG.getNode(AVRISD::SELECT_CC, dl, VTs, &Ops[0], Ops.size());
#endif
  llvm_unreachable("Not implemented");
  return SDValue();
}

SDValue AVRTargetLowering::LowerSIGN_EXTEND(SDValue Op,
                                               SelectionDAG &DAG)
{
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();
  DebugLoc dl = Op.getDebugLoc();
  
  llvm_unreachable("Not implemented");

  assert(VT == MVT::i16 && "Only support i16 for now!");

  return DAG.getNode(ISD::SIGN_EXTEND_INREG, dl, VT,
                     DAG.getNode(ISD::ANY_EXTEND, dl, VT, Val),
                     DAG.getValueType(Val.getValueType()));
}

const char *AVRTargetLowering::getTargetNodeName(unsigned Opcode) const
{
  switch (Opcode)
  {
    default: return NULL;
    case AVRISD::RET_FLAG:           return "AVRISD::RET_FLAG";
    case AVRISD::RRA:                return "AVRISD::RRA";
    case AVRISD::RLA:                return "AVRISD::RLA";
    case AVRISD::RRC:                return "AVRISD::RRC";
    case AVRISD::CALL:               return "AVRISD::CALL";
    case AVRISD::Wrapper:            return "AVRISD::Wrapper";
    case AVRISD::BR_CC:              return "AVRISD::BR_CC";
    case AVRISD::CMP:                return "AVRISD::CMP";
    case AVRISD::SELECT_CC:          return "AVRISD::SELECT_CC";
  }
}

//===----------------------------------------------------------------------===//
//  Other Lowering Code
//===----------------------------------------------------------------------===//
MachineBasicBlock *
AVRTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                                 MachineBasicBlock *BB,
                   DenseMap<MachineBasicBlock*, MachineBasicBlock*> *EM) const
{
#if 0
  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();
  DebugLoc dl = MI->getDebugLoc();
  assert((MI->getOpcode() == AVR::Select16 ||
          MI->getOpcode() == AVR::Select8) &&
         "Unexpected instr type to insert");

  // To "insert" a SELECT instruction, we actually have to insert the diamond
  // control-flow pattern.  The incoming instruction knows the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator I = BB;
  ++I;

  //  thisMBB:
  //  ...
  //   TrueVal = ...
  //   cmpTY ccX, r1, r2
  //   jCC copy1MBB
  //   fallthrough --> copy0MBB
  MachineBasicBlock *thisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *copy1MBB = F->CreateMachineBasicBlock(LLVM_BB);
  BuildMI(BB, dl, TII.get(AVR::JCC))
    .addMBB(copy1MBB)
    .addImm(MI->getOperand(3).getImm());
  F->insert(I, copy0MBB);
  F->insert(I, copy1MBB);
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi node for the select.
  copy1MBB->transferSuccessors(BB);
  // Next, add the true and fallthrough blocks as its successors.
  BB->addSuccessor(copy0MBB);
  BB->addSuccessor(copy1MBB);

  //  copy0MBB:
  //   %FalseValue = ...
  //   # fallthrough to copy1MBB
  BB = copy0MBB;

  // Update machine-CFG edges
  BB->addSuccessor(copy1MBB);

  //  copy1MBB:
  //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
  //  ...
  BB = copy1MBB;
  BuildMI(BB, dl, TII.get(AVR::PHI),
          MI->getOperand(0).getReg())
    .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB)
    .addReg(MI->getOperand(1).getReg()).addMBB(thisMBB);

  F->DeleteMachineInstr(MI);   // The pseudo instruction is gone now.
  return BB;
#endif
  llvm_unreachable("Not implemented");
  return NULL;
}
