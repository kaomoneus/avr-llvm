; RUN: llc < %s -march=avr | FileCheck %s

; TODO: test returning byval structs

declare i8 @foo8_1(i8)
declare i8 @foo8_2(i8, i8, i8)
declare i8 @foo8_3(i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8)

declare i16 @foo16_1(i16, i16)
declare i16 @foo16_2(i16, i16, i16, i16, i16, i16, i16, i16, i16, i16, i16)

declare i32 @foo32_1(i32, i32)
declare i32 @foo32_2(i32, i32, i32, i32, i32)

declare i64 @foo64_1(i64)
declare i64 @foo64_2(i64, i64, i64)

define i8 @calli8_reg() {
; CHECK: calli8_reg:
; CHECK: ldi r24, 12
; CHECK: call foo8_1
; CHECK: ldi r24, 12
; CHECK: ldi r22, 13
; CHECK: ldi r20, 14
; CHECK: call foo8_2
    %result1 = call i8 @foo8_1(i8 12)
    %result2 = call i8 @foo8_2(i8 12, i8 13, i8 14)
    ret i8 %result2
}

define i8 @calli8_stack() {
; CHECK: calli8_stack:
; CHECK: ldi [[REG:r[0-9]+]], 11
; CHECK: std [[SP:Y|Z]]+2, [[REG]]
; CHECK: ldi [[REG:r[0-9]+]], 10
; CHECK: std [[SP]]+1, [[REG]]
; CHECK: call foo8_3
    %result1 = call i8 @foo8_3(i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8, i8 9, i8 10, i8 11)
    ret i8 %result1
}

define i16 @calli16_reg() {
; CHECK: calli16_reg:
; CHECK: ldi r24, 1
; CHECK: ldi r25, 2
; CHECK: ldi r22, 2
; CHECK: ldi r23, 2
; CHECK: call foo16_1
    %result1 = call i16 @foo16_1(i16 513, i16 514)
    ret i16 %result1
}

define i16 @calli16_stack() {
; CHECK: calli16_stack:
; CHECK: ldi [[REG1:r[0-9]+]], 10
; CHECK: ldi [[REG2:r[0-9]+]], 2
; CHECK: std [[SP:Y|Z]]+3, [[REG1]]
; CHECK: std [[SP]]+4, [[REG2]]
; CHECK: ldi [[REG1:r[0-9]+]], 9
; CHECK: ldi [[REG2:r[0-9]+]], 2 
; CHECK: std [[SP]]+1, [[REG1]]
; CHECK: std [[SP]]+2, [[REG2]]
; CHECK: call foo16_2
    %result1 = call i16 @foo16_2(i16 512, i16 513, i16 514, i16 515, i16 516, i16 517, i16 518, i16 519, i16 520, i16 521, i16 522)
    ret i16 %result1
}

define i32 @calli32_reg() {
; CHECK: calli32_reg:
; CHECK: ldi r22, 64
; CHECK: ldi r23, 66
; CHECK: ldi r24, 15
; CHECK: ldi r25, 2
; CHECK: ldi r18, 128
; CHECK: ldi r19, 132
; CHECK: ldi r20, 30
; CHECK: ldi r21, 2
; CHECK: call foo32_1
    %result1 = call i32 @foo32_1(i32 34554432, i32 35554432)
    ret i32 %result1
}

define i32 @calli32_stack() {
; CHECK: calli32_stack:
; CHECK: ldi [[REG1:r[0-9]+]], 15
; CHECK: ldi [[REG2:r[0-9]+]], 2
; CHECK: std [[SP:Y|Z]]+3, [[REG1]]
; CHECK: std [[SP]]+4, [[REG2]]
; CHECK: ldi [[REG1:r[0-9]+]], 64
; CHECK: ldi [[REG2:r[0-9]+]], 66
; CHECK: std [[SP]]+1, [[REG1]]
; CHECK: std [[SP]]+2, [[REG2]]
; CHECK: call foo32_2
    %result1 = call i32 @foo32_2(i32 1, i32 2, i32 3, i32 4, i32 34554432)
    ret i32 %result1
}

define i64 @calli64_reg() {
; CHECK: calli64_reg:
; CHECK: ldi r18, 255
; CHECK: ldi r19, 255
; CHECK: ldi r20, 155
; CHECK: ldi r21, 88
; CHECK: ldi r22, 76
; CHECK: ldi r23, 73
; CHECK: ldi r24, 31
; CHECK: ldi r25, 242
; CHECK: call foo64_1
    %result1 = call i64 @foo64_1(i64 17446744073709551615)
    ret i64 %result1
}

define i64 @calli64_stack() {
; CHECK: calli64_stack:
; CHECK: ldi [[REG1:r[0-9]+]], 31
; CHECK: ldi [[REG2:r[0-9]+]], 242
; CHECK: std [[SP:Y|Z]]+7, [[REG1]]
; CHECK: std [[SP]]+8, [[REG2]]
; CHECK: ldi [[REG1:r[0-9]+]], 76
; CHECK: ldi [[REG2:r[0-9]+]], 73
; CHECK: std [[SP]]+5, [[REG1]]
; CHECK: std [[SP]]+6, [[REG2]]
; CHECK: ldi [[REG1:r[0-9]+]], 155
; CHECK: ldi [[REG2:r[0-9]+]], 88
; CHECK: std [[SP]]+3, [[REG1]]
; CHECK: std [[SP]]+4, [[REG2]]
; CHECK: ldi [[REG1:r[0-9]+]], 255
; CHECK: ldi [[REG2:r[0-9]+]], 255
; CHECK: std [[SP]]+1, [[REG1]]
; CHECK: std [[SP]]+2, [[REG2]]
; CHECK: call foo64_2
    %result1 = call i64 @foo64_2(i64 1, i64 2, i64 17446744073709551615)
    ret i64 %result1
}

declare i32 @bar(i32)

define i32 @icall(i32 (i32)* %foo) {
; CHECK: icall:
; CHECK: movw [[REG1:r[0-9]+]]:[[REG2:r[0-9]+]], r25:r24
; CHECK: ldi r22, 147
; CHECK: ldi r23, 248
; CHECK: ldi r24, 214
; CHECK: ldi r25, 198
; CHECK: movw r31:r30, [[REG1]]:[[REG2]]
; CHECK: icall
; CHECK: subi r22, 251
; CHECK: sbci r23, 255
; CHECK: sbci r24, 255
; CHECK: sbci r25, 255
  %1 = call i32 %foo(i32 3335977107)
  %2 = add nsw i32 %1, 5
  ret i32 %2
}

; Calling external functions (like __divsf3) require extra processing for
; arguments and return values in the LowerCall function.
declare i32 @foofloat(float)

define i32 @externcall(float %a, float %b) {
; CHECK: externcall:
; CHECK: movw [[REG1:r[0-9]+]]:[[REG2:r[0-9]+]], r25:r24
; CHECK: movw [[REG3:r[0-9]+]]:[[REG4:r[0-9]+]], r23:r22
; CHECK: movw r23:r22, r19:r18
; CHECK: movw r25:r24, r21:r20
; CHECK: movw r19:r18, [[REG3]]:[[REG4]]
; CHECK: movw r21:r20, [[REG1]]:[[REG2]]
; CHECK: call __divsf3
; CHECK: call foofloat
; CHECK: subi r22, 251
; CHECK: sbci r23, 255
; CHECK: sbci r24, 255
; CHECK: sbci r25, 255
  %1 = fdiv float %b, %a
  %2 = call i32 @foofloat(float %1)
  %3 = add nsw i32 %2, 5
  ret i32 %3
}
