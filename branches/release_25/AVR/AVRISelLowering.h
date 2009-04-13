//===-- AVRISelLowering.h - AVR DAG Lowering Interface ----------*- C++ -*-===//
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

#ifndef AVRISELLOWERING_H
#define AVRISELLOWERING_H

#include "AVR.h"
#include "AVRSubtarget.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
  namespace AVRISD {
    enum NodeType {
      // Start the numbering from where ISD NodeType finishes.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

      Lo,            // Low 8-bits of GlobalAddress.
      Hi,            // High 8-bits of GlobalAddress.
      AVRLoad,
      AVRLdWF,
      AVRStore,
      AVRStWF,
      Banksel,
      MTLO,
      MTHI,
      BCF,
      LSLF,          // AVR Logical shift left
      LRLF,          // AVR Logical shift right
      RLF,           // Rotate left through carry
      RRF,           // Rotate right through carry
      CALL,          // AVR Call instruction 
      SUBCC,	     // Compare for equality or inequality.
      SELECT_ICC,    // Psuedo to be caught in schedular and expanded to brcond.
      BRCOND,        // Conditional branch.
      Dummy
    };

    // Keep track of different address spaces. 
    enum AddressSpace {
      RAM_SPACE = 0,      // RAM address space
      FLASH_SPACE = 1,    // Flash memory
      EEPROM_SPACE = 2    // EEPROM memory
    };
    enum AVRLibcall {
      MUL_I8,
      SRA_I8,
      SLL_I8,
      SRL_I8,
      AVRUnknownCall
    };
  }


  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//
  class AVRTargetLowering : public TargetLowering {
  public:
    explicit AVRTargetLowering(AVRTargetMachine &TM);

    /// getTargetNodeName - This method returns the name of a target specific
    /// DAG node.
    //virtual const char *getTargetNodeName(unsigned Opcode) const;
    /// getSetCCResultType - Return the ISD::SETCC ValueType
    /*
    virtual MVT getSetCCResultType(MVT ValType) const;
    SDValue LowerFORMAL_ARGUMENTS(SDValue Op, SelectionDAG &DAG);
    SDValue LowerShift(SDValue Op, SelectionDAG &DAG);
    SDValue LowerADD(SDValue Op, SelectionDAG &DAG);
    SDValue LowerSUB(SDValue Op, SelectionDAG &DAG);
    SDValue LowerBinOp(SDValue Op, SelectionDAG &DAG);
    SDValue LowerCALL(SDValue Op, SelectionDAG &DAG);
    SDValue LowerRET(SDValue Op, SelectionDAG &DAG);
    SDValue LowerCallReturn(SDValue Op, SDValue Chain, SDValue FrameAddress,
                            SDValue InFlag, SelectionDAG &DAG);
    SDValue LowerCallArguments(SDValue Op, SDValue Chain, SDValue FrameAddress,
                               SDValue InFlag, SelectionDAG &DAG);
    SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG);
    SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG);
    SDValue getAVRCmp(SDValue LHS, SDValue RHS, unsigned OrigCC, SDValue &CC,
                        SelectionDAG &DAG);
    virtual MachineBasicBlock *EmitInstrWithCustomInserter(MachineInstr *MI,
                                                        MachineBasicBlock *MBB);
*/

    virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG);
/*  
    virtual void ReplaceNodeResults(SDNode *N,
                                    SmallVectorImpl<SDValue> &Results,
                                    SelectionDAG &DAG);
    virtual void LowerOperationWrapper(SDNode *N,
                                       SmallVectorImpl<SDValue> &Results,
                                       SelectionDAG &DAG);

    SDValue ExpandStore(SDNode *N, SelectionDAG &DAG);
    SDValue ExpandLoad(SDNode *N, SelectionDAG &DAG);
    //SDValue ExpandAdd(SDNode *N, SelectionDAG &DAG);
    SDValue ExpandGlobalAddress(SDNode *N, SelectionDAG &DAG);
    SDValue ExpandExternalSymbol(SDNode *N, SelectionDAG &DAG);
    SDValue ExpandFrameIndex(SDNode *N, SelectionDAG &DAG);

    SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const; 
    SDValue PerformAVRLoadCombine(SDNode *N, DAGCombinerInfo &DCI) const; 
*/
  private:
/*
    // If the Node is a BUILD_PAIR representing representing an Address
    // then this function will return true
    bool isDirectAddress(const SDValue &Op);

    // If the Node is a DirectAddress in ROM_SPACE then this 
    // function will return true
    bool isRomAddress(const SDValue &Op);

    // To extract chain value from the SDValue Nodes
    // This function will help to maintain the chain extracting
    // code at one place. In case of any change in future it will
    // help maintain the code
    SDValue getChain(SDValue &Op);
    
    SDValue getOutFlag(SDValue &Op);


    // Extract the Lo and Hi component of Op. 
    void GetExpandedParts(SDValue Op, SelectionDAG &DAG, SDValue &Lo, 
                          SDValue &Hi); 


    // Load pointer can be a direct or indirect address. In AVR direct
    // addresses need Banksel and Indirect addresses need to be loaded to
    // FSR first. Handle address specific cases here.
    void LegalizeAddress(SDValue Ptr, SelectionDAG &DAG, SDValue &Chain, 
                         SDValue &NewPtr, unsigned &Offset);

    // FrameIndex should be broken down into ExternalSymbol and FrameOffset. 
    void LegalizeFrameIndex(SDValue Op, SelectionDAG &DAG, SDValue &ES, 
                            int &Offset);

    // We can not have both operands of a binary operation in W.
    // This function is used to put one operand on stack and generate a load.
    SDValue ConvertToMemOperand(SDValue Op, SelectionDAG &DAG); 

    // This function checks if we need to put an operand of an operation on
    // stack and generate a load or not.
    bool NeedToConvertToMemOp(SDValue Op, unsigned &MemOp); 
*/
    /// Subtarget - Keep a pointer to the AVRSubtarget around so that we can
    /// make the right decision when generating code for different targets.
    const AVRSubtarget *Subtarget;
/*

    // Extending the LIB Call framework of LLVM
    // To hold the names of AVRLibcalls
    const char *AVRLibcallNames[AVRISD::AVRUnknownCall]; 

    // To set and retrieve the lib call names
    void setAVRLibcallName(AVRISD::AVRLibcall Call, const char *Name);
    const char *getAVRLibcallName(AVRISD::AVRLibcall Call);

    // Make AVR Libcall
    SDValue MakeAVRLibcall(AVRISD::AVRLibcall Call, MVT RetVT, 
                             const SDValue *Ops, unsigned NumOps, bool isSigned,
                             SelectionDAG &DAG, DebugLoc dl);

    // Check if operation has a direct load operand.
    inline bool isDirectLoad(const SDValue Op);

    // Create the symbol and index for function frame
    void getCurrentFrameIndex(SelectionDAG &DAG, SDValue &ES, 
                              unsigned SlotSize, int &FI);

    SDValue getCurrentFrame(SelectionDAG &DAG);
*/
  };
} // namespace llvm

#endif // AVRISELLOWERING_H
