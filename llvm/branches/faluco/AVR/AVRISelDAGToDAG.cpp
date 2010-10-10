//===------ AVRISelDAGToDAG.cpp - A dag to dag inst selector for AVR ------===//
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

#include "AVRTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"

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

// Include the pieces autogenerated from the target description.
#include "AVRGenDAGISel.inc"

  private:
    SDNode *Select(SDNode *N);
  };
}

FunctionPass *llvm::createAVRISelDag(AVRTargetMachine &TM,
                                     CodeGenOpt::Level OptLevel)
{
  return new AVRDAGToDAGISel(TM, OptLevel);
}

SDNode *AVRDAGToDAGISel::Select(SDNode *N)
{
  unsigned Opcode = N->getOpcode();
  DebugLoc dl = N->getDebugLoc();
  EVT NVT = N->getValueType(0);

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
    case ISD::SMUL_LOHI:
    case ISD::UMUL_LOHI:
      {
        SDValue Op1 = N->getOperand(0);
        SDValue Op2 = N->getOperand(1);
        unsigned LoReg = AVR::R0, HiReg = AVR::R1;
        unsigned Opc = AVR::MULRdRr;

        SDValue InFlag =
          SDValue(CurDAG->getMachineNode(Opc, dl, MVT::Flag, Op1, Op2), 0);

        // Copy the low half of the result, if it is needed.
        if (!SDValue(N, 0).use_empty())
        {
          SDValue Result = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl,
                                                  LoReg, NVT, InFlag);
          InFlag = Result.getValue(2);
          ReplaceUses(SDValue(N, 0), Result);
        }

        // Copy the high half of the result, if it is needed.
        if (!SDValue(N, 1).use_empty())
        {
          SDValue Result = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl,
                                                  HiReg, NVT, InFlag);
          InFlag = Result.getValue(2);
          ReplaceUses(SDValue(N, 1), Result);
        }

        //:TODO: remember to zero R1 after the mult
        return NULL;
      }
    case AVRISD::Wrapper:
      {
        assert(0 && "IselDAG wrapper");
      }
    default:
      break;
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
