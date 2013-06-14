; RUN: llc < %s -march=avr | FileCheck %s

;CHECK: no_operands:
define void @no_operands() {
  ;CHECK: add r24, r22
  call void asm sideeffect "add r24, r22", ""() nounwind
  ret void
}

;CHECK: input_operand:
define void @input_operand(i8 %a) {
  ;CHECK: add r24, r24
  call void asm sideeffect "add $0, $0", "r"(i8 %a) nounwind
  ret void
}

;CHECK: simple_upper_regs:
define void @simple_upper_regs(i8 %p0, i8 %p1, i8 %p2, i8 %p3,
                               i8 %p4, i8 %p5, i8 %p6, i8 %p7) {
  ;CHECK: some_instr r17, r22, r20, r18, r16, r23, r21, r19
  call void asm sideeffect "some_instr $0, $1, $2, $3, $4, $5, $6, $7",
                           "a,a,a,a,a,a,a,a" (i8 %p0, i8 %p1, i8 %p2, i8 %p3,
                                              i8 %p4, i8 %p5, i8 %p6, i8 %p7) nounwind
  ret void
}

;CHECK: upper_regs:
define void @upper_regs(i8 %p0) {
  ;CHECK: some_instr r24
  call void asm sideeffect "some_instr $0", "d" (i8 %p0) nounwind
  ret void
}

;CHECK: lower_regs:
define void @lower_regs(i8 %p0) {
  ;CHECK: some_instr r2
  call void asm sideeffect "some_instr $0", "l" (i8 %p0) nounwind
  ret void
}

;CHECK: special_upper_regs:
define void @special_upper_regs(i8 %p0, i8 %p1, i8 %p2, i8 %p3) {
  ;CHECK: some_instr r24,r28,r30,r26
  call void asm sideeffect "some_instr $0,$1,$2,$3", "w,w,w,w" (i8 %p0, i8 %p1, i8 %p2, i8 %p3) nounwind
  ret void
}

;CHECK: xyz_reg:
define void @xyz_reg(i16 %var) {
  ;CHECK: some_instr r26, r28, r30
  call void asm sideeffect "some_instr $0, $1, $2", "x,y,z" (i16 %var, i16 %var, i16 %var) nounwind
  ret void
}

;TODO
; How to use SP reg properly in inline asm??
; define void @sp_reg(i16 %var) 

;CHECK: ptr_reg:
define void @ptr_reg(i16 %var0, i16 %var1, i16 %var2) {
  ;CHECK: some_instr r28, r26, r30
  call void asm sideeffect "some_instr $0, $1, $2", "e,e,e" (i16 %var0, i16 %var1, i16 %var2) nounwind
  ret void
}

;CHECK: base_ptr_reg:
define void @base_ptr_reg(i16 %var0, i16 %var1) {
  ;CHECK: some_instr r28, r30
  call void asm sideeffect "some_instr $0, $1", "b,b" (i16 %var0, i16 %var1) nounwind
  ret void
}

;CHECK: input_output_operand:
define i8 @input_output_operand(i8 %a, i8 %b) {
  ;CHECK: add r24, r24
  %1 = call i8 asm "add $0, $1", "=r,r"(i8 %a) nounwind
  ret i8 %1
}

;CHECK: temp_reg:
define void @temp_reg(i8 %a) {
  ;CHECK: some_instr r0
  call void asm sideeffect "some_instr $0", "t" (i8 %a) nounwind
  ret void
}

;CHECK: int_0_63:
define void @int_0_63() {
  ;CHECK: some_instr 5
  call void asm sideeffect "some_instr $0", "I" (i8 5) nounwind
  ret void
}

;CHECK: int_minus63_0:
define void @int_minus63_0() {
  ;CHECK: some_instr -5
  call void asm sideeffect "some_instr $0", "J" (i8 -5) nounwind
  ret void
}

;CHECK: int_2_2:
define void @int_2_2() {
  ;CHECK: some_instr 2
  call void asm sideeffect "some_instr $0", "K" (i8 2) nounwind
  ret void
}

;CHECK: int_0_0:
define void @int_0_0() {
  ;CHECK: some_instr 0
  call void asm sideeffect "some_instr $0", "L" (i8 0) nounwind
  ret void
}

;CHECK: int_0_255:
define void @int_0_255() {
  ;CHECK: some_instr 254
  call void asm sideeffect "some_instr $0", "M" (i8 254) nounwind
  ret void
}

;CHECK: int_minus1_minus1:
define void @int_minus1_minus1() {
  ;CHECK: some_instr -1
  call void asm sideeffect "some_instr $0", "N" (i8 -1) nounwind
  ret void
}

;CHECK: int_8_or_16_or_24:
define void @int_8_or_16_or_24() {
  ;CHECK: some_instr 8, 16, 24
  call void asm sideeffect "some_instr $0, $1, $2", "O,O,O" (i8 8, i8 16, i8 24) nounwind
  ret void
}

;CHECK: int_1_1:
define void @int_1_1() {
  ;CHECK: some_instr 1
  call void asm sideeffect "some_instr $0", "P" (i8 1) nounwind
  ret void
}

;CHECK: int_minus6_5:
define void @int_minus6_5() {
  ;CHECK: some_instr -6
  call void asm sideeffect "some_instr $0", "R" (i8 -6) nounwind
  ret void
}

;CHECK: float_0_0:
define void @float_0_0() {
  ;CHECK: some_instr 0
  call void asm sideeffect "some_instr $0", "G" (float 0.0) nounwind
  ret void
}
