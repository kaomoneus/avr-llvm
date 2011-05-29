

Driver.diff causes clang to look for avr-gcc instead of gcc.
Only apply this patch if the AVR is the only target being built.
