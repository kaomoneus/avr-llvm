#!/bin/bash

# run script from inside root
# Project needs to be build first with all targets enabled using ./configure
# Creates file LLVMLibDeps.cmake.replace
# sed script from comments at bottom of llvm/cmake/modules/LLVM-Config.cmake

./utils/UpdateCMakeLists.pl
sed -e s'#\.a##g' -e 's#libLLVM#LLVM#g' -e 's#: # #' -e 's#\(.*\)#set(MSVC_LIB_DEPS_\1)#' tools/llvm-config/LibDeps.txt > LLVMLibDeps.cmake.replace


# Now just [ $ mv LLVMLibDeps.cmake.replace cmake/modules/LLVMLibDeps.cmake ]

