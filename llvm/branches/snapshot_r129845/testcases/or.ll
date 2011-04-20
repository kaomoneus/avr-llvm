


define i8 @func_or(i8 %a, i8 %b) {
entry:
	%c = or i8 %a, %b
	ret i8 %c
}


define i8 @func_or_imm(i8 %a) {
    %tmp = or i8 %a,5
    ret i8 %tmp
}


define i8 @func_xor(i8 %a, i8 %b) {
entry:
	%c = xor i8 %a, %b
	ret i8 %c
}


define i8 @func_xor_imm(i8 %a) {
    %tmp = xor i8 %a,5
    ret i8 %tmp
}