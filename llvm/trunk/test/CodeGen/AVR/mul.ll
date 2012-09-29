; RUN: llc < %s -march=avr | FileCheck %s

define i8 @mult8(i8 %a, i8 %b) {
; CHECK: mult8:
; CHECK: mul r22, r24
; CHECK: mov r24, r0
; :TODO: clr r1
  %mul = mul i8 %b, %a
  ret i8 %mul
}

define i16 @mult16(i16 %a, i16 %b) {
; CHECK: mult16:
; CHECK: movw r18, r24
; CHECK: mul r22, r18
; CHECK: movw r24, r0
; CHECK: mul r22, r19
; CHECK: add r25, r0
; CHECK: mul r23, r18
; CHECK: add r25, r0
; :TODO: clr r1
  %mul = mul nsw i16 %b, %a
  ret i16 %mul
}
