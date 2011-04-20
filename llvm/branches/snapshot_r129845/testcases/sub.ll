target triple = "avr-unknown-unknown"


define i8 @foo(i8 %a, i8 %b) {
entry:
	%c = sub i8 %a, %b
	ret i8 %c
}

