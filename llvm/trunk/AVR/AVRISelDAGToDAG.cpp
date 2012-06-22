//===-- AVRISelDAGToDAG.cpp - A dag to dag inst selector for AVR ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the AVR target.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "avr-isel"

#include "AVR.h"
#include "AVRTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

namespace
{

class AVRDAGToDAGISel : public SelectionDAGISel
{
public:
  explicit AVRDAGToDAGISel(AVRTargetMachine &tm, CodeGenOpt::Level OptLevel) :
    SelectionDAGISel(tm, OptLevel) {}

  const char *getPassName() const
  {
    return "AVR DAG->DAG Instruction Selection";
  }

  // Address Selection
  bool SelectAddr(SDValue N, SDValue &Base, SDValue &Disp);
  // Indexed load (postinc and predec) matching code.
  SDNode *SelectIndexedLoad(SDNode *N);
  // Indexed progmem load (only postinc) matching code.
  unsigned SelectIndexedProgMemLoad(const LoadSDNode *LD, MVT VT);

  // Include the pieces autogenerated from the target description.
  #include "AVRGenDAGISel.inc"

private:
  SDNode *Select(SDNode *N);
};

} // end of anonymous namespace

bool AVRDAGToDAGISel::SelectAddr(SDValue N, SDValue &Base, SDValue &Disp)
{
  // if N (the address) is a FI get the TargetFrameIndex.
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N))
  {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), TLI.getPointerTy());
    Disp = CurDAG->getTargetConstant(0, MVT::i8);

    return true;
  }

  // Match simple Reg + uimm6 operands.
  if ((N.getOpcode() != ISD::ADD) && (N.getOpcode() != ISD::SUB)
      && !CurDAG->isBaseWithConstantOffset(N))
  {
    return false;
  }

  if (ConstantSDNode *RHS = dyn_cast<ConstantSDNode>(N.getOperand(1)))
  {
    int RHSC = (int)RHS->getZExtValue();
    // Convert negative offsets into positives ones.
    if (N.getOpcode() == ISD::SUB)
    {
      RHSC = -RHSC;
    }

    // <#FI + const>
    // Allow folding offsets bigger than 63 so the frame pointer can be used
    // directly instead of copying it around by adjusting and restoring it for
    // each access.
    if (N.getOperand(0).getOpcode() == ISD::FrameIndex)
    {
      int FI = cast<FrameIndexSDNode>(N.getOperand(0))->getIndex();
      Base = CurDAG->getTargetFrameIndex(FI, TLI.getPointerTy());
      Disp = CurDAG->getTargetConstant(RHSC, MVT::i8);

      return true;
    }

    // We only accept offsets that fit in 6 bits (unsigned).
    if ((N.getValueType() == MVT::i8 && RHSC >= 0 && RHSC < 64)
        || (N.getValueType() == MVT::i16 && RHSC >= 0 && RHSC < 63))
    {
      Base = N.getOperand(0);
      Disp = CurDAG->getTargetConstant(RHSC, MVT::i8);

      return true;
    }
  }

  return false;
}

SDNode *AVRDAGToDAGISel::SelectIndexedLoad(SDNode *N)
{
  const LoadSDNode *LD = cast<LoadSDNode>(N);
  ISD::MemIndexedMode AM = LD->getAddressingMode();
  MVT VT = LD->getMemoryVT().getSimpleVT();

  if ((LD->getExtensionType() != ISD::NON_EXTLOAD)
      || (AM != ISD::POST_INC && AM != ISD::PRE_DEC))
  {
    return NULL;
  }

  unsigned Opcode = 0;
  bool isPre = (AM == ISD::PRE_DEC);
  int Offs = cast<ConstantSDNode>(LD->getOffset())->getSExtValue();
  switch (VT.SimpleTy)
  {
  case MVT::i8:
    if ((!isPre && Offs != 1) || (isPre && Offs != -1))
    {
      return NULL;
    }
    Opcode = (isPre) ? AVR::LDRdPtrPd : AVR::LDRdPtrPi;
    break;
  case MVT::i16:
    if ((!isPre && Offs != 2) || (isPre && Offs != -2))
    {
      return NULL;
    }
    Opcode = (isPre) ? AVR::LDWRdPtrPd : AVR::LDWRdPtrPi;
    break;
  default:
    return NULL;
  }

  return CurDAG->getMachineNode(Opcode, N->getDebugLoc(), VT,
                                TLI.getPointerTy(), MVT::Other,
                                LD->getBasePtr(), LD->getChain());
}

unsigned AVRDAGToDAGISel::SelectIndexedProgMemLoad(const LoadSDNode *LD, MVT VT)
{
  ISD::MemIndexedMode AM = LD->getAddressingMode();

  if ((LD->getExtensionType() != ISD::NON_EXTLOAD) || (AM != ISD::POST_INC))
  {
    return 0;
  }

  unsigned Opcode = 0;
  int Offs = cast<ConstantSDNode>(LD->getOffset())->getSExtValue();
  switch (VT.SimpleTy)
  {
  case MVT::i8:
    if (Offs != 1)
    {
      return 0;
    }
    Opcode = AVR::LPMRdZPi;
    break;
  case MVT::i16:
    if (Offs != 2)
    {
      return 0;
    }
    Opcode = AVR::LPMWRdZPi;
    break;
  default:
    return 0;
  }

  return Opcode;
}

SDNode *AVRDAGToDAGISel::Select(SDNode *N)
{
  unsigned Opcode = N->getOpcode();
  DebugLoc dl = N->getDebugLoc();

  // Dump information about the Node being selected
  DEBUG(errs() << "Selecting: "; N->dump(CurDAG); errs() << "\n");

  // If we have a custom node, we already have selected!
  if (N->isMachineOpcode())
  {
    DEBUG(errs() << "== "; N->dump(CurDAG); errs() << "\n");
    return NULL;
  }

  switch (Opcode)
  {
  case ISD::FrameIndex:
    {
      // Convert the frameindex into a temp instruction that will hold the
      // effective address of the final stack slot.
      int FI = cast<FrameIndexSDNode>(N)->getIndex();
      SDValue TFI = CurDAG->getTargetFrameIndex(FI, TLI.getPointerTy());

      return CurDAG->SelectNodeTo(N, AVR::FRMIDX, TLI.getPointerTy(), TFI,
                                  CurDAG->getTargetConstant(0, MVT::i16));
    }
  case ISD::LOAD:
    {
      const LoadSDNode *LD = cast<LoadSDNode>(N);
      if (cast<PointerType>(LD->getSrcValue()->getType())->getAddressSpace()==1)
      {
        // This is a flash memory load, move the pointer into R31R30 and emit
        // the lpm instruction.
        MVT VT = LD->getMemoryVT().getSimpleVT();
        SDValue Chain = LD->getChain();
        SDValue Ptr = LD->getBasePtr();
        SDNode *ResNode;

        Chain = CurDAG->getCopyToReg(Chain, dl, AVR::R31R30, Ptr, SDValue(0,0));
        Ptr = CurDAG->getCopyFromReg(Chain, dl, AVR::R31R30, MVT::i16,
                                     Chain.getValue(1));

        // Check if the opcode can be converted into an indexed load.
        if (unsigned LPMOpc = SelectIndexedProgMemLoad(LD, VT))
        {
          // It is legal to fold the load into an indexed load.
          ResNode = CurDAG->getMachineNode(LPMOpc, dl, VT, MVT::i16, MVT::Other,
                                           Ptr, Ptr.getValue(1));
          ReplaceUses(SDValue(N, 2), SDValue(ResNode, 2));
        }
        else
        {
          // Not legal to select an indexed load, fallback to a normal load
          switch (VT.SimpleTy)
          {
          case MVT::i8:
            ResNode = CurDAG->getMachineNode(AVR::LPMRdZ, dl, MVT::i8,
                                             MVT::Other, Ptr, Ptr.getValue(1));
            break;
          case MVT::i16:
            ResNode = CurDAG->getMachineNode(AVR::LPMWRdZ, dl, MVT::i16,
                                             MVT::i16, MVT::Other, Ptr,
                                             Ptr.getValue(1));
            ReplaceUses(SDValue(N, 2), SDValue(ResNode, 2));
            break;
          default:
            llvm_unreachable("Unsupported VT!");
          }
        }

        // Transfer memoperands.
        MachineSDNode::mmo_iterator MemOp = MF->allocateMemRefsArray(1);
        MemOp[0] = LD->getMemOperand();
        cast<MachineSDNode>(ResNode)->setMemRefs(MemOp, MemOp + 1);

        ReplaceUses(SDValue(N, 0), SDValue(ResNode, 0));
        ReplaceUses(SDValue(N, 1), SDValue(ResNode, 1));

        return ResNode;
      }

      // Check if the opcode can be converted into an indexed load
      if (SDNode *ResNode = SelectIndexedLoad(N))
      {
        return ResNode;
      }
      // Other cases are autogenerated.
      break;
    }
  case AVRISD::CALL:
    {
      // Handle indirect calls because ICALL can only take R31R30 as its source
      // operand
      SDValue InFlag;
      SDValue Chain = N->getOperand(0);
      SDValue Callee = N->getOperand(1);
      unsigned LastOpNum = N->getNumOperands() - 1;

      // Direct calls are autogenerated
      if (Callee.getOpcode() == ISD::TargetGlobalAddress
          || Callee.getOpcode() == ISD::TargetExternalSymbol)
      {
        break;
      }

      // Skip the incoming flag if present
      if (N->getOperand(LastOpNum).getValueType() == MVT::Glue)
      {
        LastOpNum--;
      }

      Chain = CurDAG->getCopyToReg(Chain, dl, AVR::R31R30, Callee, InFlag);
      SmallVector<SDValue, 8> Ops;
      Ops.push_back(CurDAG->getRegister(AVR::R31R30, MVT::i16));

      // Map all operands into the new node
      for (unsigned i = 2, e = LastOpNum + 1; i != e; ++i)
      {
        Ops.push_back(N->getOperand(i));
      }
      Ops.push_back(Chain);
      Ops.push_back(Chain.getValue(1));

      SDNode *ResNode = CurDAG->getMachineNode(AVR::ICALL, dl, MVT::Other,
                                               MVT::Glue, &Ops[0], Ops.size());

      ReplaceUses(SDValue(N, 0), SDValue(ResNode, 0));
      ReplaceUses(SDValue(N, 1), SDValue(ResNode, 1));

      return ResNode;
    }
  case ISD::BRIND:
    {
      // Move the destination address of the indirect branch into R31R30.
      SDValue Chain = N->getOperand(0);
      SDValue JmpAddr = N->getOperand(1);

      Chain = CurDAG->getCopyToReg(Chain, dl, AVR::R31R30, JmpAddr);
      SDNode *ResNode = CurDAG->getMachineNode(AVR::IJMP, dl, MVT::Other,Chain);

      ReplaceUses(SDValue(N, 0), SDValue(ResNode, 0));

      return ResNode;
    }
  }

  SDNode *ResNode = SelectCode(N);

  DEBUG(errs() << "=> ";
        if (ResNode == NULL || ResNode == N)
        {
          N->dump(CurDAG);
        }
        else
        {
          ResNode->dump(CurDAG);
        }
        errs() << "\n";
       );

  return ResNode;
}

/// createAVRISelDag - This pass converts a legalized DAG into a
/// AVR-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createAVRISelDag(AVRTargetMachine &TM,
                                     CodeGenOpt::Level OptLevel)
{
  return new AVRDAGToDAGISel(TM, OptLevel);
}
