##===- lib/Target/AVR/Makefile -----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMAVRCodeGen
TARGET = AVR

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = AVRGenRegisterInfo.h.inc AVRGenRegisterNames.inc \
                AVRGenRegisterInfo.inc AVRGenInstrNames.inc \
                AVRGenInstrInfo.inc AVRGenDAGISel.inc \
                AVRGenCallingConv.inc AVRGenAsmWriter.inc \
                AVRGenSubtarget.inc

DIRS = InstPrinter TargetInfo

include $(LEVEL)/Makefile.common
