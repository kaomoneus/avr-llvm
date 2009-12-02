; ModuleID = '../avr-llvm/trunk/testcases/add1.c'
target triple = "avr-unknown-unknown"

define zeroext i8 @add() nounwind {
entry:
  %retval = alloca i32                            ; <i32*> [#uses=2]
  %b = alloca i8                                  ; <i8*> [#uses=2]
  %a = alloca i8                                  ; <i8*> [#uses=2]
 ; %0 = alloca i32                                 ; <i32*> [#uses=2]
 ; %"alloca point" = bitcast i32 0 to i32          ; <i32> [#uses=0]
  store i8 5, i8* %a, align 1
  store i8 7, i8* %b, align 1
 ; %1 = load i8* %a, align 1                       ; <i8> [#uses=1]
 ; %2 = load i8* %b, align 1                       ; <i8> [#uses=1]
 ; %3 = add i8 %1, %2                              ; <i8> [#uses=1]
 ; %4 = zext i8 %3 to i32                          ; <i32> [#uses=1]
 ; store i32 %4, i32* %0, align 4
 ; %5 = load i32* %0, align 4                      ; <i32> [#uses=1]
 ; store i32 %5, i32* %retval, align 4
 ; br label %return

;return:                                           ; preds = %entry
 ; %retval1 = load i32* %retval                    ; <i32> [#uses=1]
 ; %retval12 = trunc i32 %retval1 to i8            ; <i8> [#uses=1]
 ; ret i8 %retval12
  %retval1 = load i8* %a, align 1                       ; <i8> [#uses=1]
  ret i8 %retval1
}
