

Driver.diff:
  causes clang to look for avr-gcc instead of gcc.
Only apply this patch if the AVR is the only target being built.

tools.svnprops:
  Import this into the TortioseSVN properties of the tools folder to add clang
as an external project. An update of LLVM will add/update clang also.
Linux/configure doesn't really need this since it has 'make update'.