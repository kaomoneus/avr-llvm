target triple = "avr-unknown-unknown"

define i8 @func_add_imm(i8 %a) {
  %tmp = add i8 %a,5
  ret i8 %tmp
}