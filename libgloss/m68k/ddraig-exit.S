#include "asm.h"

	.title "ddraig-exit.S"

	.text

	.extern SYM (__ddraig_saved_sp)

/*
 * _exit -- leave the loaded program and return to the shell loader.
 *
 * The loader enters the ELF image with a normal C call, so on entry to
 * _start the original stack already contains the loader return address and
 * arguments.  ddraig_crt0.S saves that stack pointer before calling main.
 *
 * exit() and _Exit() eventually land here.  Restore the original loader
 * stack, leave the exit status in d0 as the function return value, and
 * return directly to load_elf_executable().
 */
	.global SYM (_exit)
SYM (_exit):
	movel	4(sp), d0
	movel	SYM (__ddraig_saved_sp), sp
	rts
