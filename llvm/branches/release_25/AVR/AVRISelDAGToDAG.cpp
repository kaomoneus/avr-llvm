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

#include "AVRISelDAGToDAG.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

/// createAVRISelDag - This pass converts a legalized DAG into a
/// AVR-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createAVRISelDag(AVRTargetMachine &TM) {
  return new AVRDAGToDAGISel(TM);
}


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
