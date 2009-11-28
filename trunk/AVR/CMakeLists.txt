set(LLVM_TARGET_DEFINITIONS AVR.td)

tablegen(AVRGenRegisterInfo.h.inc -gen-register-desc-header)
tablegen(AVRGenRegisterNames.inc -gen-register-enums)
tablegen(AVRGenRegisterInfo.inc -gen-register-desc)
tablegen(AVRGenInstrNames.inc -gen-instr-enums)
tablegen(AVRGenInstrInfo.inc -gen-instr-desc)
tablegen(AVRGenAsmWriter.inc -gen-asm-writer)
tablegen(AVRGenDAGISel.inc -gen-dag-isel)
tablegen(AVRGenCallingConv.inc -gen-callingconv)
tablegen(AVRGenSubtarget.inc -gen-subtarget)

add_llvm_target(AVRCodeGen
  AVRInstrInfo.cpp
  AVRISelDAGToDAG.cpp
  AVRISelLowering.cpp
  AVRMCAsmInfo.cpp
  AVRRegisterInfo.cpp
  AVRSubtarget.cpp
  AVRTargetMachine.cpp
  )

target_link_libraries (LLVMAVRCodeGen LLVMSelectionDAG)
