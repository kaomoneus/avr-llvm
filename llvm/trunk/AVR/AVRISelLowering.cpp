//===-- AVRISelLowering.cpp - AVR DAG Lowering Implementation -------------===//
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

#include "AVR.h"
#include "AVRISelLowering.h"
#include "AVRTargetMachine.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

AVRTargetLowering::AVRTargetLowering(AVRTargetMachine &tm) :
  TargetLowering(tm, new TargetLoweringObjectFileELF()), TM(tm)
{
  TD = getTargetData();

  // Set up the register classes.
  addRegisterClass(MVT::i8, AVR::GPR8RegisterClass);
  addRegisterClass(MVT::i16, AVR::DREGSRegisterClass);

  // Compute derived properties from the register classes
  computeRegisterProperties();

  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent); // FIXME: Is this correct?
  setSchedulingPreference(Sched::RegPressure);

  setOperationAction(ISD::GlobalAddress, getPointerTy(), Custom);

  setLoadExtAction(ISD::EXTLOAD, MVT::i8, Expand);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8, Expand);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i8, Expand);

  // sub (x, imm) gets canonicalized to add (x, -imm), so for illegal types
  // revert into a sub since we don't have an add with immediate instruction
  setOperationAction(ISD::ADD, MVT::i32, Custom);
  setOperationAction(ISD::ADD, MVT::i64, Custom);

  // our shift instructions are only able to shift 1 bit at a time, so handle
  // this in a custom way
  setOperationAction(ISD::SRA, MVT::i8, Custom);
  setOperationAction(ISD::SHL, MVT::i8, Custom);
  setOperationAction(ISD::SRL, MVT::i8, Custom);
  setOperationAction(ISD::SRA, MVT::i16, Custom);
  setOperationAction(ISD::SHL, MVT::i16, Custom);
  setOperationAction(ISD::SRL, MVT::i16, Custom);

  setOperationAction(ISD::BR_CC, MVT::i8, Custom);
  setOperationAction(ISD::BR_CC, MVT::i16, Custom);
  setOperationAction(ISD::BR_CC, MVT::i32, Custom);
  setOperationAction(ISD::BR_CC, MVT::i64, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i8, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i64, Custom);
  setOperationAction(ISD::SETCC, MVT::i8, Expand);
  setOperationAction(ISD::SETCC, MVT::i16, Expand);

  // add support for postincrement and predecrement load/stores
  setIndexedLoadAction(ISD::POST_INC, MVT::i8, Legal);
  setIndexedLoadAction(ISD::POST_INC, MVT::i16, Legal);
  setIndexedLoadAction(ISD::PRE_DEC, MVT::i8, Legal);
  setIndexedLoadAction(ISD::PRE_DEC, MVT::i16, Legal);
  setIndexedStoreAction(ISD::POST_INC, MVT::i8, Legal);
  setIndexedStoreAction(ISD::POST_INC, MVT::i16, Legal);
  setIndexedStoreAction(ISD::PRE_DEC, MVT::i8, Legal);
  setIndexedStoreAction(ISD::PRE_DEC, MVT::i16, Legal);

  setMinFunctionAlignment(1);
}

const char *AVRTargetLowering::getTargetNodeName(unsigned Opcode) const
{
  switch (Opcode)
  {
  default:                                    return NULL;
  case AVRISD::RET_FLAG:                      return "AVRISD::RET_FLAG";
  case AVRISD::CALL:                          return "AVRISD::CALL";
  case AVRISD::Wrapper:                       return "AVRISD::Wrapper";
  case AVRISD::LSL:                           return "AVRISD::LSL";
  case AVRISD::LSR:                           return "AVRISD::LSR";
  case AVRISD::ROL:                           return "AVRISD::ROL";
  case AVRISD::ROR:                           return "AVRISD::ROR";
  case AVRISD::ASR:                           return "AVRISD::ASR";
  case AVRISD::SPLOAD:                        return "AVRISD::SPLOAD";
  case AVRISD::BRCOND:                        return "AVRISD::BRCOND";
  case AVRISD::CMP:                           return "AVRISD::CMP";
  case AVRISD::CMPC:                          return "AVRISD::CMPC";
  case AVRISD::TST:                           return "AVRISD::TST";
  case AVRISD::SELECT_CC:                     return "AVRISD::SELECT_CC";
  }
}

/// IntCCToAVRCC - Convert a DAG integer condition code to an AVR CC
static AVRCC::CondCodes IntCCToAVRCC(ISD::CondCode CC)
{
  switch (CC)
  {
  default:                      llvm_unreachable("Unknown condition code!");
  case ISD::SETEQ:              return AVRCC::COND_EQ;
  case ISD::SETNE:              return AVRCC::COND_NE;
  case ISD::SETGE:              return AVRCC::COND_GE;
  case ISD::SETLT:              return AVRCC::COND_LT;
  case ISD::SETUGE:             return AVRCC::COND_SH;
  case ISD::SETULT:             return AVRCC::COND_LO;
  }
}

SDValue AVRTargetLowering::LowerShifts(SDValue Op, SelectionDAG &DAG) const
{
  //:TODO: this function has to be completely rewritten to produce optimal
  // code, for now it's producing very long but correct code.
  unsigned Opc8;
  const SDNode *N = Op.getNode();
  EVT VT = Op.getValueType();
  DebugLoc dl = N->getDebugLoc();
  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
  SDValue Victim = N->getOperand(0);

  switch (Op.getOpcode())
  {
  case ISD::SRA:
    Opc8 = AVRISD::ASR;
    break;
  case ISD::ROTL:
    Opc8 = AVRISD::ROL;
    break;
  case ISD::ROTR:
    Opc8 = AVRISD::ROR;
    break;
  case ISD::SRL:
    Opc8 = AVRISD::LSR;
    break;
  case ISD::SHL:
    Opc8 = AVRISD::LSL;
    break;
  default:
    llvm_unreachable("Invalid shift opcode");
  }

  while (ShiftAmount--)
  {
    Victim = DAG.getNode(Opc8, dl, VT, Victim);
  }

  return Victim;
}

SDValue
AVRTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();

  // Create the TargetGlobalAddress node, folding in the constant offset.
  SDValue Result = DAG.getTargetGlobalAddress(GV, Op.getDebugLoc(),
                                              getPointerTy(), Offset);
  return DAG.getNode(AVRISD::Wrapper, Op.getDebugLoc(), getPointerTy(), Result);
}

/// Returns appropriate AVR CMP/CMPC nodes and corresponding condition code for
/// the given operands.
SDValue AVRTargetLowering::getAVRCmp(SDValue LHS, SDValue RHS, ISD::CondCode CC,
                                     SDValue &AVRcc, SelectionDAG &DAG,
                                     DebugLoc dl) const
{
  SDValue Cmp;
  unsigned CmpSize = LHS.getValueSizeInBits();
  bool useTest = false;

  switch (CC)
  {
  default:
    break;
  case ISD::SETLE:
    {
      // Swap operands and reverse the branching condition
      std::swap(LHS, RHS);
      CC = ISD::SETGE;
      break;
    }
  case ISD::SETGT:
    {
      if (const ConstantSDNode *C = dyn_cast<ConstantSDNode>(RHS))
      {
        switch (C->getSExtValue())
        {
        case -1:
          {
            // When doing lhs > -1 use a tst instruction on the top part of lhs
            // and use brpl instead of using a chain of cp/cpc
            useTest = true;
            AVRcc = DAG.getConstant(AVRCC::COND_PL, MVT::i8);
            break;
          }
        case 0:
          {
            // Turn lhs > 0 into 0 < lhs since 0 can be materialized with
            // __zero_reg__ in lhs
            RHS = LHS;
            LHS = DAG.getConstant(0, C->getValueType(0));
            CC = ISD::SETLT;
            break;
          }
        default:
          {
            // Turn lhs < rhs with lhs constant into rhs >= lhs+1, this allows
            // us to  fold the constant into the cmp instruction.
            RHS = DAG.getConstant(C->getZExtValue() + 1, C->getValueType(0));
            CC = ISD::SETGE;
            break;
          }
        }
        break;
      }
      // Swap operands and reverse the branching condition
      std::swap(LHS, RHS);
      CC = ISD::SETLT;
      break;
    }
  case ISD::SETLT:
    {
      if (const ConstantSDNode *C = dyn_cast<ConstantSDNode>(RHS))
      {
        switch (C->getZExtValue())
        {
        case 1:
          {
            // Turn lhs < 1 into 0 >= lhs since 0 can be materialized with
            // __zero_reg__ in lhs
            RHS = LHS;
            LHS = DAG.getConstant(0, C->getValueType(0));
            CC = ISD::SETGE;
            break;
          }
        case 0:
          {
            // When doing lhs < 0 use a tst instruction on the top part of lhs
            // and use brmi instead of using a chain of cp/cpc
            useTest = true;
            AVRcc = DAG.getConstant(AVRCC::COND_MI, MVT::i8);
            break;
          }
        }
      }
      break;
    }
  case ISD::SETULE:
    {
      // Swap operands and reverse the branching condition
      std::swap(LHS, RHS);
      CC = ISD::SETUGE;
      break;
    }
  case ISD::SETUGT:
    {
      // Turn lhs < rhs with lhs constant into rhs >= lhs+1, this allows us to
      // fold the constant into the cmp instruction.
      if (const ConstantSDNode *C = dyn_cast<ConstantSDNode>(RHS))
      {
        RHS = DAG.getConstant(C->getZExtValue() + 1, C->getValueType(0));
        CC = ISD::SETUGE;
        break;
      }
      // Swap operands and reverse the branching condition
      std::swap(LHS, RHS);
      CC = ISD::SETULT;
      break;
    }
  }

  // Expand 32 and 64 bit comparisons with custom CMP and CMPC nodes instead of
  // using the default and/or/xor expansion code which is much larger.
  if (CmpSize == 32)
  {
    SDValue LHSlo = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS,
                                DAG.getIntPtrConstant(0));
    SDValue LHShi = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS,
                                DAG.getIntPtrConstant(1));
    SDValue RHSlo = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS,
                                DAG.getIntPtrConstant(0));
    SDValue RHShi = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS,
                                DAG.getIntPtrConstant(1));

    if (useTest)
    {
      // When using tst we only care about the highest part
      SDValue Top = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i8, LHShi,
                                DAG.getIntPtrConstant(1));
      Cmp = DAG.getNode(AVRISD::TST, dl, MVT::Glue, Top);
    }
    else
    {
      Cmp = DAG.getNode(AVRISD::CMP, dl, MVT::Glue, LHSlo, RHSlo);
      Cmp = DAG.getNode(AVRISD::CMPC, dl, MVT::Glue, LHShi, RHShi, Cmp);
    }
  }
  else if (CmpSize == 64)
  {
    SDValue LHS_0 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, LHS,
                                DAG.getIntPtrConstant(0));
    SDValue LHS_1 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, LHS,
                                DAG.getIntPtrConstant(1));

    SDValue LHS0 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS_0,
                               DAG.getIntPtrConstant(0));
    SDValue LHS1 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS_0,
                               DAG.getIntPtrConstant(1));
    SDValue LHS2 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS_1,
                               DAG.getIntPtrConstant(0));
    SDValue LHS3 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, LHS_1,
                               DAG.getIntPtrConstant(1));

    SDValue RHS_0 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, RHS,
                                DAG.getIntPtrConstant(0));
    SDValue RHS_1 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, RHS,
                                DAG.getIntPtrConstant(1));

    SDValue RHS0 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS_0,
                               DAG.getIntPtrConstant(0));
    SDValue RHS1 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS_0,
                               DAG.getIntPtrConstant(1));
    SDValue RHS2 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS_1,
                               DAG.getIntPtrConstant(0));
    SDValue RHS3 = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i16, RHS_1,
                               DAG.getIntPtrConstant(1));

    if (useTest)
    {
      // When using tst we only care about the highest part
      SDValue Top = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i8, LHS3,
                                DAG.getIntPtrConstant(1));
      Cmp = DAG.getNode(AVRISD::TST, dl, MVT::Glue, Top);
    }
    else
    {
      Cmp = DAG.getNode(AVRISD::CMP, dl, MVT::Glue, LHS0, RHS0);
      Cmp = DAG.getNode(AVRISD::CMPC, dl, MVT::Glue, LHS1, RHS1, Cmp);
      Cmp = DAG.getNode(AVRISD::CMPC, dl, MVT::Glue, LHS2, RHS2, Cmp);
      Cmp = DAG.getNode(AVRISD::CMPC, dl, MVT::Glue, LHS3, RHS3, Cmp);
    }
  }
  else if (CmpSize == 8 || CmpSize == 16)
  {
    if (useTest)
    {
      // When using tst we only care about the highest part
      Cmp = DAG.getNode(AVRISD::TST, dl, MVT::Glue,
        (CmpSize == 8) ? LHS : DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i8,
                                           LHS, DAG.getIntPtrConstant(1)));
    }
    else
    {
      Cmp = DAG.getNode(AVRISD::CMP, dl, MVT::Glue, LHS, RHS);
    }
  }
  else
  {
    llvm_unreachable("Invalid comparison size");
  }

  // When using a test instruction AVRcc is already set
  if (!useTest)
  {
    AVRcc = DAG.getConstant(IntCCToAVRCC(CC), MVT::i8);
  }

  return Cmp;
}

SDValue AVRTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const
{
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  DebugLoc dl = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Cmp = getAVRCmp(LHS, RHS, CC, TargetCC, DAG, dl);

  return DAG.getNode(AVRISD::BRCOND, dl, MVT::Other, Chain, Dest, TargetCC,
                     Cmp);
}

SDValue AVRTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const
{
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueV = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  DebugLoc dl = Op.getDebugLoc();

  SDValue TargetCC;
  SDValue Cmp = getAVRCmp(LHS, RHS, CC, TargetCC, DAG, dl);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SmallVector<SDValue, 4> Ops;
  Ops.push_back(TrueV);
  Ops.push_back(FalseV);
  Ops.push_back(TargetCC);
  Ops.push_back(Cmp);

  return DAG.getNode(AVRISD::SELECT_CC, dl, VTs, &Ops[0], Ops.size());
}

SDValue AVRTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode())
  {
  default:
    llvm_unreachable("Don't know how to custom lower this!");
    break;
  case ISD::SHL:
  case ISD::SRA:
  case ISD::SRL:
  case ISD::ROTL:
  case ISD::ROTR:
    return LowerShifts(Op, DAG);
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSELECT_CC(Op, DAG);
  }

  return SDValue();
}

/// ReplaceNodeResults - Replace a node with an illegal result type
/// with a new node built out of custom code.
void AVRTargetLowering::ReplaceNodeResults(SDNode *N,
                                           SmallVectorImpl<SDValue> &Results,
                                           SelectionDAG &DAG) const
{
  DebugLoc dl = N->getDebugLoc();

  switch (N->getOpcode())
  {
  default:
    llvm_unreachable("Don't know how to custom expand this!");
    break;
  case ISD::ADD:
    {
      // convert add (x, imm) into sub (x, -imm)
      if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(N->getOperand(1)))
      {
        uint64_t value = C->getZExtValue();
        SDValue sub =
          DAG.getNode(ISD::SUB, dl, N->getValueType(0), N->getOperand(0),
                      DAG.getConstant(-value, C->getValueType(0)));
        Results.push_back(sub);
        return;
      }
      break;
    }
  }
}

/// isLegalAddressingMode - Return true if the addressing mode represented
/// by AM is legal for this target, for a load/store of the specified type.
bool AVRTargetLowering::isLegalAddressingMode(const AddrMode &AM,
                                              Type *Ty) const
{
  int64_t Offs = AM.BaseOffs;

  // allow absolute addresses.
  if (AM.BaseGV && !AM.HasBaseReg && AM.Scale == 0 && Offs == 0)
  {
    return true;
  }

  // allow reg+<6bit> offset.
  if (Offs < 0) Offs = -Offs;
  if (AM.BaseGV == 0 && AM.HasBaseReg && AM.Scale == 0 && isUInt<6>(Offs))
  {
    return true;
  }

  return false;
}

/// getPreIndexedAddressParts - returns true by value, base pointer and
/// offset pointer and addressing mode by reference if the node's address
/// can be legally represented as pre-indexed load / store address.
bool AVRTargetLowering::getPreIndexedAddressParts(SDNode *N, SDValue &Base,
                                                  SDValue &Offset,
                                                  ISD::MemIndexedMode &AM,
                                                  SelectionDAG &DAG) const
{
  EVT VT;
  const SDNode *Op;

  if (LoadSDNode *LD = dyn_cast<LoadSDNode>(N))
  {
    VT = LD->getMemoryVT();
    Op = LD->getBasePtr().getNode();
    if (LD->getExtensionType() != ISD::NON_EXTLOAD) return false;
  }
  else if (StoreSDNode *ST = dyn_cast<StoreSDNode>(N))
  {
    VT = ST->getMemoryVT();
    Op = ST->getBasePtr().getNode();
  }
  else
  {
    return false;
  }

  if (VT != MVT::i8 && VT != MVT::i16)
  {
    return false;
  }

  if (Op->getOpcode() != ISD::ADD && Op->getOpcode() != ISD::SUB)
  {
    return false;
  }

  if (ConstantSDNode *RHS = dyn_cast<ConstantSDNode>(Op->getOperand(1)))
  {
    int RHSC = RHS->getSExtValue();
    if (Op->getOpcode() == ISD::SUB) RHSC = -RHSC;
    if ((VT == MVT::i16 && RHSC != -2) || (VT == MVT::i8 && RHSC != -1))
    {
      return false;
    }

    Base = Op->getOperand(0);
    Offset = DAG.getConstant(RHSC, MVT::i8);
    AM = ISD::PRE_DEC;

    return true;
  }

  return false;
}

/// getPostIndexedAddressParts - returns true by value, base pointer and
/// offset pointer and addressing mode by reference if this node can be
/// combined with a load / store to form a post-indexed load / store.
bool AVRTargetLowering::getPostIndexedAddressParts(SDNode *N, SDNode *Op,
                                                   SDValue &Base,
                                                   SDValue &Offset,
                                                   ISD::MemIndexedMode &AM,
                                                   SelectionDAG &DAG) const
{
  EVT VT;

  if (LoadSDNode *LD = dyn_cast<LoadSDNode>(N))
  {
    VT  = LD->getMemoryVT();
    if (LD->getExtensionType() != ISD::NON_EXTLOAD) return false;
  }
  else if (StoreSDNode *ST = dyn_cast<StoreSDNode>(N))
  {
    VT  = ST->getMemoryVT();
  }
  else
  {
    return false;
  }

  if (VT != MVT::i8 && VT != MVT::i16)
  {
    return false;
  }

  if (Op->getOpcode() != ISD::ADD && Op->getOpcode() != ISD::SUB)
  {
    return false;
  }

  if (ConstantSDNode *RHS = dyn_cast<ConstantSDNode>(Op->getOperand(1)))
  {
    int RHSC = RHS->getSExtValue();
    if (Op->getOpcode() == ISD::SUB) RHSC = -RHSC;
    if ((VT == MVT::i16 && RHSC != 2) || (VT == MVT::i8 && RHSC != 1))
    {
      return false;
    }

    Base = Op->getOperand(0);
    Offset = DAG.getConstant(RHSC, MVT::i8);
    AM = ISD::POST_INC;

    return true;
  }

  return false;
}

/// isOffsetFoldingLegal - Return true if folding a constant offset
/// with the given GlobalAddress is legal.  It is frequently not legal in
/// PIC relocation models.
bool
AVRTargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const
{
  //:TODO: consider folding other operators like or,and,xor,...
  return true;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "AVRGenCallingConv.inc"

/// For each argument in a function store the number of pieces it is composed of
static void ParseFunctionArgs(const Function *F, const TargetData *TD,
                              SmallVectorImpl<unsigned> &Out)
{
  for (Function::const_arg_iterator I = F->arg_begin(), E = F->arg_end();
       I != E; ++I)
  {
    unsigned Bytes = TD->getTypeSizeInBits(I->getType()) / 8;
    Out.push_back(((Bytes == 1) || (Bytes == 2)) ? 1 : Bytes / 2);
  }
}

/// For external symbols there is no function prototype information so we
/// have to rely directly on argument sizes.
static void ParseExternFuncCallArgs(const SmallVectorImpl<ISD::OutputArg> &In,
                                    SmallVectorImpl<unsigned> &Out)
{
  for (unsigned i = 0, e = In.size(); i != e; )
  {
    unsigned Size = 0;
    while ((i != e) && (In[i].Flags.getSplitPiece() == Size))
    {
      ++i;
      ++Size;
    }
    Out.push_back(Size);
  }
}

/// Analyze incoming and outgoing function arguments. We need custom C++ code
/// to handle special constraints in the ABI like reversing the order of the
/// pieces of splitted arguments. In addition, all pieces of a certain argument
/// have to be passed either using registers or the stack but not mixing both.
static void AnalyzeArguments(const Function *F, const TargetData *TD,
                             const SmallVectorImpl<ISD::OutputArg> *Outs,
                             const SmallVectorImpl<ISD::InputArg> *Ins,
                             SmallVectorImpl<CCValAssign> &ArgLocs,
                             CCState &CCInfo, bool IsCall)
{
  static const unsigned RegList8[] =
  {
    AVR::R24, AVR::R22, AVR::R20, AVR::R18, AVR::R16, AVR::R14, AVR::R12,
    AVR::R10, AVR::R8
  };
  static const unsigned RegList16[] =
  {
    AVR::R25R24, AVR::R23R22, AVR::R21R20, AVR::R19R18, AVR::R17R16,
    AVR::R15R14, AVR::R13R12, AVR::R11R10, AVR::R9R8
  };

  SmallVector<unsigned, 8> Args;
  if (IsCall && (F == NULL))
  {
    ParseExternFuncCallArgs(*Outs, Args);
  }
  else
  {
    ParseFunctionArgs(F, TD, Args);
  }

  unsigned RegsLeft = array_lengthof(RegList8), ValNo = 0;
  bool UsesStack = false;
  for (unsigned i = 0, pos = 0, e = Args.size(); i != e; ++i)
  {
    unsigned Size = Args[i];
    MVT LocVT = (IsCall) ? (*Outs)[pos].VT : (*Ins)[pos].VT;

    // If we have plenty of regs to pass the whole argument do it
    if (!UsesStack && (Size <= RegsLeft))
    {
      const unsigned *RegList = (LocVT == MVT::i16) ? RegList16 : RegList8;

      for (unsigned j = 0; j != Size; ++j)
      {
        unsigned Reg = CCInfo.AllocateReg(RegList, array_lengthof(RegList8));
        CCInfo.addLoc(CCValAssign::getReg(ValNo++, LocVT, Reg, LocVT,
                                          CCValAssign::Full));
        --RegsLeft;
      }

      // Reverse the order of the pieces to agree with the "big endian" format
      // required in the calling convention ABI
      std::reverse(ArgLocs.begin() + pos, ArgLocs.begin() + pos + Size);
    }
    else
    {
      // Pass the rest of arguments using the stack
      UsesStack = true;
      for (unsigned j = 0; j != Size; ++j)
      {
        unsigned Offset = CCInfo.AllocateStack(TD->getTypeAllocSize(EVT(LocVT)
                              .getTypeForEVT(CCInfo.getContext())),
                            TD->getABITypeAlignment(EVT(LocVT)
                              .getTypeForEVT(CCInfo.getContext())));
        CCInfo.addLoc(CCValAssign::getMem(ValNo++, LocVT, Offset, LocVT,
                                          CCValAssign::Full));
      }
    }
    pos += Size;
  }
}

SDValue AVRTargetLowering::
LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                     const SmallVectorImpl<ISD::InputArg> &Ins, DebugLoc dl,
                     SelectionDAG &DAG,
                     SmallVectorImpl<SDValue> &InVals) const
{
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), ArgLocs, *DAG.getContext());

  AnalyzeArguments(MF.getFunction(), TD, NULL, &Ins, ArgLocs, CCInfo, false);

  assert(!isVarArg && "Varargs not supported yet");

  SDValue ArgValue;
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i)
  {
    CCValAssign &VA = ArgLocs[i];

    // Arguments stored on registers
    if (VA.isRegLoc())
    {
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i8)
      {
        RC = AVR::GPR8RegisterClass;
      }
      else if (RegVT == MVT::i16)
      {
        RC = AVR::DREGSRegisterClass;
      }
      else
      {
        llvm_unreachable("Unknown argument type!");
      }

      unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
      ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);

      // :NOTE: Clang should not promote any i8 into i16 but for safety the
      // following code will handle zexts or sexts generated by other
      // front ends. Otherwise:
      // If this is an 8 bit value, it is really passed promoted
      // to 16 bits. Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      switch (VA.getLocInfo())
      {
      default:
        llvm_unreachable("Unknown loc info!");
        break;
      case CCValAssign::Full:
        break;
      case CCValAssign::BCvt:
        ArgValue = DAG.getNode(ISD::BITCAST, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::SExt:
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      case CCValAssign::ZExt:
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
        break;
      }

      InVals.push_back(ArgValue);
    }
    else
    {
      // sanity check
      assert(VA.isMemLoc());

      EVT LocVT = VA.getLocVT();

      // Create the frame index object for this incoming parameter
      int FI = MFI->CreateFixedObject(LocVT.getSizeInBits() / 8,
                                      VA.getLocMemOffset(), true);

      // Create the SelectionDAG nodes corresponding to a load
      // from this parameter
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
      InVals.push_back(DAG.getLoad(LocVT, dl, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(FI), false,
                                   false, false, 0));
    }
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
AVRTargetLowering::LowerCall(SDValue Chain, SDValue Callee,
                             CallingConv::ID CallConv, bool isVarArg,
                             bool doesNotRet, bool &isTailCall,
                             const SmallVectorImpl<ISD::OutputArg> &Outs,
                             const SmallVectorImpl<SDValue> &OutVals,
                             const SmallVectorImpl<ISD::InputArg> &Ins,
                             DebugLoc dl, SelectionDAG &DAG,
                             SmallVectorImpl<SDValue> &InVals) const
{
  // AVR does not yet support tail call optimization
  isTailCall = false;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), ArgLocs, *DAG.getContext());

  // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol
  // node so that legalize doesn't hack it.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
  {
    const GlobalValue *GV = G->getGlobal();
    AnalyzeArguments(cast<Function>(GV), TD, &Outs, NULL, ArgLocs, CCInfo,
                     true);

    Callee = DAG.getTargetGlobalAddress(GV, dl, getPointerTy());
  }
  else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee))
  {
    AnalyzeArguments(NULL, TD, &Outs, NULL, ArgLocs, CCInfo, true);

    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), getPointerTy());
  }

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true));

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;

  SDValue SP = DAG.getRegister(AVR::SP, getPointerTy());
  SDVTList SPNodeTys = DAG.getVTList(getPointerTy(), MVT::Other, MVT::Glue);

  // Load SP from the I/O space into a physical register used as the pointer
  // to pass arguments on the stack.
  if (NumBytes)
  {
    StackPtr = DAG.getNode(AVRISD::SPLOAD, dl, SPNodeTys, Chain, SP,
                           Chain.getValue(1));
    Chain = StackPtr.getValue(1);
  }

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i)
  {
    CCValAssign &VA = ArgLocs[i];
    EVT RegVT = VA.getLocVT();
    SDValue Arg = OutVals[i];

    // Promote the value if needed. With Clang this should not happen.
    switch (VA.getLocInfo())
    {
    default:
      llvm_unreachable("Unknown loc info!");
      break;
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
    case CCValAssign::BCvt:
      Arg = DAG.getNode(ISD::BITCAST, dl, RegVT, Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at RegsToPass
    // vector
    if (VA.isRegLoc())
    {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    }
    else
    {
      assert(VA.isMemLoc());
      assert(StackPtr.getNode() != 0 && "Empty stack pointer node");

      // SP points to one stack slot further so add one to adjust it
      SDValue PtrOff = DAG.getNode(ISD::ADD, dl, getPointerTy(), StackPtr,
                                   DAG.getIntPtrConstant(VA.getLocMemOffset()
                                                         + 1));

      MemOpChains.push_back
        (DAG.getStore(Chain, dl, Arg, PtrOff,
                      MachinePointerInfo::getStack(VA.getLocMemOffset()), false,
                      false, 0));
    }
  }

  // Transform all store nodes into one single node because all store nodes are
  // independent of each other.
  if (!MemOpChains.empty())
  {
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, &MemOpChains[0],
                        MemOpChains.size());
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emited instructions must be stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
  {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
  {
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));
  }

  if (InFlag.getNode())
  {
    Ops.push_back(InFlag);
  }

  Chain = DAG.getNode(AVRISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  SDValue SPCopy = DAG.getUNDEF(getPointerTy());
  // Reload SP to a physical register to restore the stack after
  // the function call.
  if (NumBytes)
  {
    SPCopy = DAG.getNode(AVRISD::SPLOAD, dl, SPNodeTys, Chain, SP, InFlag);
    Chain = SPCopy.getValue(1);
    InFlag = SPCopy.getValue(2);
  }

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, true),
                             SPCopy, InFlag);

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
                 getTargetMachine(), RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_AVR);

  // Reverse splitted return values to get the "big endian" format required
  // to agree with the calling convention ABI.
  if (RVLocs.size() > 1)
  {
    std::reverse(RVLocs.begin(), RVLocs.end());
  }

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0, e = RVLocs.size(); i != e; ++i)
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
  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                 getTargetMachine(), RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_AVR);

  // If this is the first return lowered for this function, add the regs to
  // the liveout set for the function.
  MachineRegisterInfo &MRI = DAG.getMachineFunction().getRegInfo();
  unsigned e = RVLocs.size();

  // Reverse splitted return values to get the "big endian" format required
  // to agree with the calling convention ABI.
  if (e > 1)
  {
    std::reverse(RVLocs.begin(), RVLocs.end());
  }

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
//  Other Lowering Code
//===----------------------------------------------------------------------===//

MachineBasicBlock *
AVRTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                               MachineBasicBlock *BB) const
{
  unsigned Opc = MI->getOpcode();
  assert((Opc == AVR::Select16 || Opc == AVR::Select8)
         && "Unexpected instr type to insert");

  const AVRInstrInfo &TII = *TM.getInstrInfo();
  DebugLoc dl = MI->getDebugLoc();

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
  AVRCC::CondCodes CC = (AVRCC::CondCodes)MI->getOperand(3).getImm();
  F->insert(I, copy0MBB);
  F->insert(I, copy1MBB);
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi node for the select.
  copy1MBB->splice(copy1MBB->begin(), BB,
                   llvm::next(MachineBasicBlock::iterator(MI)),
                   BB->end());
  copy1MBB->transferSuccessorsAndUpdatePHIs(BB);
  // Next, add the true and fallthrough blocks as its successors.
  BB->addSuccessor(copy0MBB);
  BB->addSuccessor(copy1MBB);

  BuildMI(BB, dl, TII.getBrCond(CC)).addMBB(copy1MBB);

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
  BuildMI(*BB, BB->begin(), dl, TII.get(AVR::PHI),
          MI->getOperand(0).getReg())
    .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB)
    .addReg(MI->getOperand(1).getReg()).addMBB(thisMBB);

  MI->eraseFromParent();   // The pseudo instruction is gone now.
  return BB;
}
