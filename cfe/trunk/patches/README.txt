

Driver.patch:
  causes clang to look for avr-gcc instead of gcc.
Only apply this patch if AVR is the only target being built.

tools.svnprops:
  Import this into the TortioseSVN properties of the tools folder to add clang
as an external project. An update of LLVM will add/update clang also.
Can be helpfull when using Visual Studio/Cmake but when using configure 
doing a 'make update' will accomplish the same thing.
