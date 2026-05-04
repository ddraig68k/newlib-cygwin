#include "asm.h"

	.title "ddraig-exit.S"

	.text

	.extern SYM (__ddraig_saved_sp)
	.extern SYM (__ddraig_saved_regs)

/*
 * _exit -- leave the loaded program and return to the shell loader.
 *
 * The loader enters the ELF image with a normal C call, so on entry to
 * _start the original stack already contains the loader return address and
 * arguments.  ddraig_crt0.S saves that stack pointer before calling main.
 *
 * exit() and _Exit() eventually land here.  Restore the original loader
 * register context and stack, leave the exit status in d0 as the function
 * return value, and return directly to load_elf_executable().
 */
	.global SYM (_exit)
SYM (_exit):
	movel	4(sp), d0
	moveml	SYM (__ddraig_saved_regs), d2-d7/a2-a6
	movel	SYM (__ddraig_saved_sp), sp
	rts
