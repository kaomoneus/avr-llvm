//===-- AVRAsmPrinter.cpp - AVR LLVM assembly writer ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to AVR assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "AVRAsmPrinter.h"
//#include "AVRSection.h"
#include "AVRMCAsmInfo.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Mangler.h"
#include <cstring>

/*
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/MCAsmInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Mangler.h"
//#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MathExtras.h"
//#include <cctype>
//#include <cstring>
//#include <map>
*/

using namespace llvm;

// STATISTIC(EmittedInsts, "Number of machine instrs printed");


using namespace llvm;

AVRAsmPrinter::AVRAsmPrinter(formatted_raw_ostream &O, TargetMachine &TM, 
                             const MCAsmInfo *T, bool V)
                             : AsmPrinter(O, TM, T, V) { }

#if 0
/// createAVRCodePrinterPass - Returns a pass that prints the AVR
/// assembly code for a MachineFunction to the given output stream,
/// using the given target machine description.  This should work
/// regardless of whether the function is in SSA form.
///
FunctionPass *llvm::createAVRCodePrinterPass(raw_ostream &o,
                                             AVRTargetMachine &tm,
                                             CodeGenOpt::Level OptLevel,
                                             bool verbose) {
  return new AVRAsmPrinter(o, tm, tm.getMCAsmInfo(), OptLevel, verbose);
}

//TODO delete me (PIC stuff?)
/*
bool AVRAsmPrinter::inSameBank (char *s1, char *s2){

  assert (s1 && s2 && "Null pointer assignment");

  if ((*s1 == '.') && (*s2 == '.')) { //skip if they both start with '.'
    s1++;
    s2++;
  }
  while (*s1 && *s2) {
    if (*s1 != *s2)
      goto _NotInSameBank;

    if ((*s1 == '.') && (*s2 == '.')) //both symbols in same function
      goto _InSameBank;               //in the same bank

    s1++;
    s2++;
  }

  if (*s1 && *s1) {
  _InSameBank:
    return true;
  }

 _NotInSameBank:
  return false;
}*/

bool AVRAsmPrinter::printMachineInstruction(const MachineInstr *MI) {
  std::string NewBankselLabel;
  unsigned Operands = MI->getNumOperands();
  if (Operands > 1) {
    // Global address or external symbol should be second operand from last
    // if we want to print banksel for it.
    const MachineOperand &Op = MI->getOperand(Operands-2);
    unsigned OpType = Op.getType();
    if (OpType == MachineOperand::MO_GlobalAddress ||
        OpType == MachineOperand::MO_ExternalSymbol) {
      if (OpType == MachineOperand::MO_GlobalAddress )
        NewBankselLabel =  Mang->getValueName(Op.getGlobal());
      else
        NewBankselLabel =  Op.getSymbolName();

      // Operand after global address or external symbol should be  banksel.
      // Value 1 for this operand means we need to generate banksel else do not
      // generate banksel.
      const MachineOperand &BS = MI->getOperand(Operands-1);
      if (((int)BS.getImm() == 1) &&
          (!inSameBank ((char *)CurrentBankselLabelInBasicBlock.c_str(),
			(char *)NewBankselLabel.c_str()))) {
        CurrentBankselLabelInBasicBlock = NewBankselLabel;
        O << "\tbanksel ";
        printOperand(MI, Operands-2);
        O << "\n";
      }
    }
  }
  printInstruction(MI);
  return true;
}
#endif
/// runOnMachineFunction - This uses the printInstruction()
/// method to print assembly for each instruction.
///
bool AVRAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
  // This calls the base class function required to be called at beginning
  // of runOnMachineFunction.
  SetupMachineFunction(MF);
#if 0
  // Get the mangled name.
  const Function *F = MF.getFunction();
  CurrentFnName = Mang->getValueName(F);
  // TODO change to getMangledName


  // Emit the function variables.
  emitFunctionData(MF);
  std::string codeSection;
  codeSection = "code." + CurrentFnName + ".# " + "CODE";
  const Section *fCodeSection = TAI->getNamedSection(codeSection.c_str(),
                                               SectionFlags::Code);
  O <<  "\n";
  SwitchToSection (fCodeSection);

  // Print out code for the function.
  for (MachineFunction::const_iterator I = MF.begin(), E = MF.end();
       I != E; ++I) {
    // Print a label for the basic block.
    if (I != MF.begin()) {
      printBasicBlockLabel(I, true);
      O << '\n';
    }
    else
      O << CurrentFnName << ":\n";
    CurrentBankselLabelInBasicBlock = "";
    for (MachineBasicBlock::const_iterator II = I->begin(), E = I->end();
         II != E; ++II) {
      // Print the assembly for the instruction.
        printMachineInstruction(II);
    }
  }
#endif
  return false;  // we didn't modify anything.
}

#if 0

void AVRAsmPrinter::printMemOperand(const MachineInstr *MI, int opNum) {
  printOperand(MI, opNum);
}

void  AVRAsmPrinter::PrintGlobalVariable(const GlobalVariable *GVar) {
}

#endif
void AVRAsmPrinter::printOperand(const MachineInstr *MI, int opNum) {
#if 0
  const MachineOperand &MO = MI->getOperand(opNum);
  switch (MO.getType()) {
    case MachineOperand::MO_Register:
      if (TargetRegisterInfo::isPhysicalRegister(MO.getReg()))
        O << TM.getRegisterInfo()->get(MO.getReg()).AsmName;
      else
        assert(0 && "not implemented");
        return;

    case MachineOperand::MO_Immediate:
      O << (int)MO.getImm();
      return;

    case MachineOperand::MO_GlobalAddress:
      O << Mang->getValueName(MO.getGlobal());
      break;

    case MachineOperand::MO_ExternalSymbol:
      O << MO.getSymbolName();
      break;

    case MachineOperand::MO_MachineBasicBlock:
      printBasicBlockLabel(MO.getMBB());
      return;

    default:
      assert(0 && " Operand type not supported.");
  }
#endif
}
#if 0
void AVRAsmPrinter::printCCOperand(const MachineInstr *MI, int opNum) {
  //int CC = (int)MI->getOperand(opNum).getImm();
  //O << AVRCondCodeToString((AVRCC::CondCodes)CC);
}


bool AVRAsmPrinter::doInitialization (Module &M) {
  bool Result = AsmPrinter::doInitialization(M);
/* ---PIC STUFF----
  // FIXME:: This is temporary solution to generate the include file.
  // The processor should be passed to llc as in input and the header file
  // should be generated accordingly.
  O << "\t#include P16F1937.INC\n";
  EmitExternsAndGlobals (M);
  EmitInitData (M);
  EmitUnInitData(M);
  //EmitRomData(M);
*/
  return Result;
}

void AVRAsmPrinter::EmitExternsAndGlobals (Module &M) {
#if 0
 // Emit declarations for external functions.
  O << "section.0" <<"\n";
  for (Module::iterator I = M.begin(), E = M.end(); I != E; I++) {
    std::string Name = Mang->getValueName(I);
    if (Name.compare("abort") == 0)
      continue;
    if (I->isDeclaration()) {
      O << "\textern " <<Name << "\n";
      O << "\textern " << Name << ".retval\n";
      O << "\textern " << Name << ".args\n";
    }
    else if (I->hasExternalLinkage()) {
      O << "\tglobal " << Name << "\n";
      O << "\tglobal " << Name << ".retval\n";
      O << "\tglobal " << Name << ".args\n";
    }
  }

  // Emit header file to include declaration of library functions
  O << "\t#include C16IntrinsicCalls.INC\n";

  // Emit declarations for external globals.
  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; I++) {
    std::string Name = Mang->getValueName(I);
    if (I->isDeclaration())
      O << "\textern "<< Name << "\n";
    else if (I->getLinkage() == GlobalValue::CommonLinkage)
      O << "\tglobal "<< Name << "\n";
  }
#endif
}
void AVRAsmPrinter::EmitInitData (Module &M) {
/*  SwitchToSection(TAI->getDataSection());
  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I) {
    if (!I->hasInitializer())   // External global require no code.
      continue;

    Constant *C = I->getInitializer();
    const PointerType *PtrTy = I->getType();
    int AddrSpace = PtrTy->getAddressSpace();

    if ((!C->isNullValue()) && (AddrSpace == AVRISD::RAM_SPACE)) {

      if (EmitSpecialLLVMGlobal(I))
        continue;

      // Any variables reaching here with "." in its name is a local scope
      // variable and should not be printed in global data section.
      std::string name = Mang->getValueName(I);
      if (name.find(".") != std::string::npos)
        continue;

      O << name;
      EmitGlobalConstant(C, AddrSpace);
    }
  }*/
}
/*
void AVRAsmPrinter::EmitRomData (Module &M)
{
  SwitchToSection(TAI->getReadOnlySection());
  IsRomData = true;
  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I) {
    if (!I->hasInitializer())   // External global require no code.
      continue;

    Constant *C = I->getInitializer();
    const PointerType *PtrTy = I->getType();
    int AddrSpace = PtrTy->getAddressSpace();
    if ((!C->isNullValue()) && (AddrSpace == AVRISD::ROM_SPACE)) {

      if (EmitSpecialLLVMGlobal(I))
        continue;

      // Any variables reaching here with "." in its name is a local scope
      // variable and should not be printed in global data section.
      std::string name = Mang->getValueName(I);
      if (name.find(".") != std::string::npos)
        continue;

      O << name;
      EmitGlobalConstant(C, AddrSpace);
      O << "\n";
    }
  }
  IsRomData = false;
}
*/
void AVRAsmPrinter::EmitUnInitData (Module &M)
{

  assert(0 && "Not yet implemented!");
 /* SwitchToSection(TAI->getBSSSection_());
  const TargetData *TD = TM.getTargetData();

  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I) {
    if (!I->hasInitializer())   // External global require no code.
      continue;

    Constant *C = I->getInitializer();
    if (C->isNullValue()) {

      if (EmitSpecialLLVMGlobal(I))
        continue;

      // Any variables reaching here with "." in its name is a local scope
      // variable and should not be printed in global data section.
      std::string name = Mang->getValueName(I);
      if (name.find(".") != std::string::npos)
        continue;

      const Type *Ty = C->getType();
      unsigned Size = TD->getTypePaddedSize(Ty);

      O << name << " " <<"RES"<< " " << Size ;
      O << "\n";
    }
  }*/
}

bool AVRAsmPrinter::doFinalization(Module &M) {
  O << "\t" << "END\n";
  bool Result = AsmPrinter::doFinalization(M);
  return Result;
}

void AVRAsmPrinter::emitFunctionData(MachineFunction &MF) {

  assert(0 && "Not Yet Implemented");
/*

  const Function *F = MF.getFunction();
  std::string FuncName = Mang->getValueName(F);
  const Module *M = F->getParent();
  const TargetData *TD = TM.getTargetData();
  unsigned FrameSize = 0;
  // Emit the data section name.
  O << "\n";
  std::string SectionName = "fdata." + CurrentFnName + ".# " + "UDATA";

  const Section *fDataSection = TAI->getNamedSection(SectionName.c_str(),
                                               SectionFlags::Writeable);
  SwitchToSection(fDataSection);

  //Emit function return value.
  O << CurrentFnName << ".retval:\n";
  const Type *RetType = F->getReturnType();
  unsigned RetSize = 0;
  if (RetType->getTypeID() != Type::VoidTyID)
    RetSize = TD->getTypePaddedSize(RetType);

  // Emit function arguments.
  O << CurrentFnName << ".args:\n";
  // Emit the function variables.

  // In AVR all the function arguments and local variables are global.
  // Therefore to get the variable belonging to this function entire
  // global list will be traversed and variables belonging to this function
  // will be emitted in the current data section.
  for (Module::const_global_iterator I = M->global_begin(), E = M->global_end();
       I != E; ++I) {
    std::string VarName = Mang->getValueName(I);

    // The variables of a function are of form FuncName.* . If this variable
    // does not belong to this function then continue.
    if (!(VarName.find(FuncName + ".") == 0 ? true : false))
      continue;

    Constant *C = I->getInitializer();
    const Type *Ty = C->getType();
    unsigned Size = TD->getTypePaddedSize(Ty);
    FrameSize += Size;
    // Emit memory reserve directive.
    O << VarName << "  RES  " << Size << "\n";
  }
  emitFunctionTempData(MF, FrameSize);
  if (RetSize > FrameSize)
    O << CurrentFnName << ".dummy" << "RES" << (RetSize - FrameSize);
*/
}

void AVRAsmPrinter::emitFunctionTempData(MachineFunction &MF,
                                           unsigned &FrameSize) {

  assert(0 && "Not Yet Implemented");
/*
  // Emit temporary variables.
  MachineFrameInfo *FrameInfo = MF.getFrameInfo();
  if (FrameInfo->hasStackObjects()) {
    int indexBegin = FrameInfo->getObjectIndexBegin();
    int indexEnd = FrameInfo->getObjectIndexEnd();

    if (indexBegin < indexEnd) {
      FrameSize += indexEnd - indexBegin;
      O << CurrentFnName << ".tmp RES"<< " "
        <<indexEnd - indexBegin <<"\n";
    }
    while (indexBegin < indexEnd) {
        O << CurrentFnName << "_tmp_" << indexBegin << " " << "RES"<< " "
          << 1 << "\n" ;
        indexBegin++;
    }
   /
  }*/
}
#endif

#include "AVRGenAsmWriter.inc"

// Force static initialization.
extern "C" void LLVMInitializeAVRAsmPrinter() { 
  RegisterAsmPrinter<AVRAsmPrinter> X(TheAVRTarget);
}
