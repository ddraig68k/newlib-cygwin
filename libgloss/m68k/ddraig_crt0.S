/*
 * crt0.S -- startup file for y Ddraig
 *
 * Copyright (c) 2022 Stephen Moody
 */

#include "asm.h"

	.title "ddraig-crt0.S"

/*
 * Define the process environment pointer and saved loader stack pointer.
 * The shell loader calls the ELF entry point like a normal C function, so
 * the original stack can be restored later by _exit().
 */
	.data
	.align 2
	.global SYM (environ)
SYM (environ):
	.long 0

	.align 2
	.global SYM (__ddraig_saved_sp)
SYM (__ddraig_saved_sp):
	.long 0

	.align 2
	.global SYM (__ddraig_saved_regs)
SYM (__ddraig_saved_regs):
	.space 44

	.align 2
	.text

/*
 * These symbols are defined in C code, so they need to always be
 * named with SYM because of the difference between object file formats.
 */

/* These are defined in C code. */
	.extern SYM (main)
	.extern SYM (exit)
	.extern SYM (atexit)
	.extern SYM(__do_global_dtors)
	.extern __INIT_SECTION__
	.extern __FINI_SECTION__

/* 
 * These values are set in the linker script, so they must be
 * explicitly named here without SYM.
 */
	.extern __stack
	.extern __bss_start
	.extern _end

/*
 * Set things up so the application will run. For historical reasons
 * this is called 'start'.  We set things up to provide '_start'
 * as with other systems, but also provide a weak alias called
 * 'start' for compatibility with existing linker scripts.
 */
	.global SYM (start)
	.weak SYM (start)
	.set SYM (start),SYM(_start)

	.global SYM (_start)
SYM (_start):

	/*
	 * Save the loader context so _exit() can return to it later.
	 *
	 * _exit() does not unwind through main back to this function, so it
	 * must restore the registers that the m68k ABI says a callee preserves.
	 */
	movel	sp, SYM (__ddraig_saved_sp)
	moveml	d2-d7/a2-a6, SYM (__ddraig_saved_regs)

	/* Reserve stack space and capture argc/argv/envp from the loader. */
	link	a6, IMM(-12)
	movel	8(a6), d0	/* argc */
	movel	12(a6), d1	/* argv */
	movel	16(a6), a0	/* envp */
	movel	d0, -4(a6)
	movel	d1, -8(a6)
	movel	a0, -12(a6)

#ifdef ADD_DTORS
	/* Put __do_global_dtors in the atexit list so the destructors get run. */
	movel	IMM (SYM(__do_global_dtors)), (sp)
	PICCALL	SYM (atexit)
#endif
	movel	IMM (__FINI_SECTION__), (sp)
	PICCALL	SYM (atexit)

	PICCALL	__INIT_SECTION__

	/* Publish envp globally and pass argc/argv/envp to main. */
	movel	-12(a6), a0
	PICLEA	SYM (environ), a1
	movel	a0, a1@
	movel	a0, sp@-
	movel	-8(a6), sp@-
	movel	-4(a6), sp@-

	PICCALL	SYM (main)
	lea	(12, sp), sp

	movel	d0, sp@-
	PICCALL	SYM (exit)
