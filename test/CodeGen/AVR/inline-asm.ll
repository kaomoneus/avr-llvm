; RUN: llc < %s -march=avr | FileCheck %s


define void @no_operands() {
  ; CHECK: add r24, r22
  call void asm sideeffect "add r24, r22", ""() nounwind
  ret void
}

define void @input_operand(i8 %a) {
  ; CHECK: add r24, r24
  call void asm sideeffect "add $0, $0", "r"(i8 %a) nounwind
  ret void
}

define i8 @input_output_operand(i8 %a, i8 %b) {
  ; CHECK: add r24, r24
  %1 = call i8 asm "add $0, $1", "=r,r"(i8 %a) nounwind
  ret i8 %1
}

