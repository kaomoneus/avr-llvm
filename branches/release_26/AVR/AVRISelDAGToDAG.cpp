//===-- AVRISelDAGToDAG.cpp - A dag to dag inst selector for AVR ---------===//
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

//#include "AVRISelDAGToDAG.h"
#include "AVR.h"
#include "AVRISelLowering.h"
#include "AVRTargetMachine.h"
//#include "llvm/CallingConv.h"
//#include "llvm/Constants.h"
//#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
//#include "llvm/Intrinsics.h"
//#include "llvm/LLVMContext.h"
//#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
//#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;

class AVRDAGToDAGISel : public SelectionDAGISel {
  AVRTargetMachine &TM;

  /// Subtarget - Keep a pointer to the ARMSubtarget around so that we can
  /// make the right decision when generating code for different targets.
  const AVRSubtarget *Subtarget;

public:
  explicit AVRDAGToDAGISel(AVRTargetMachine &tm)
    : SelectionDAGISel(tm), TM(tm),
    Subtarget(&TM.getSubtarget<AVRSubtarget>()) {
  }
};
/*
/// createAVRISelDag - This pass converts a legalized DAG into a
/// AVR-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createAVRISelDag(AVRTargetMachine &TM) {
  return new AVRDAGToDAGISel(TM);
}
*/
#if 0
/// InstructionSelect - This callback is invoked by
/// SelectionDAGISel when it has created a SelectionDAG for us to codegen.
void AVRDAGToDAGISel::InstructionSelect() {
  DEBUG(BB->dump());
  SelectRoot(*CurDAG);
  CurDAG->RemoveDeadNodes();
}

/// Select - Select instructions not customized! Used for
/// expanded, promoted and normal instructions.
SDNode* AVRDAGToDAGISel::Select(SDValue N) {

  // Select the default instruction.
  SDNode *ResNode = SelectCode(N);

  return ResNode;
}


// SelectIOAddr - Match a IO address for DAG.
// A direct address could be a globaladdress or externalsymbol.
bool AVRDAGToDAGISel::SelectIOAddr(SDValue Op, SDValue N,
                                      SDValue &Address) {
  // Return true if TGA or ES.
  if (N.getOpcode() == ISD::TargetGlobalAddress
      || N.getOpcode() == ISD::TargetExternalSymbol) {
    Address = N;
    return true;
  }

  return false;
}
#endif
