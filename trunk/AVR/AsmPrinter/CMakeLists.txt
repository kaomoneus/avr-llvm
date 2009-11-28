include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMAVRAsmPrinter
  AVRAsmPrinter.cpp
  AVRInstPrinter.cpp
  AVRMCInstLower.cpp
  )
add_dependencies(LLVMAVRAsmPrinter AVRCodeGenTable_gen)

