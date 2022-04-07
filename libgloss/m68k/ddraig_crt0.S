/*
 * crt0.S -- startup file for y Ddraig
 *
 * Copyright (c) 2022 Stephen Moody
 */

#include "asm.h"

	.title "ddraig-crt0.S"

/*
 * These symbols are defined in C code, so they need to always be
 * named with SYM because of the difference between object file formats.
 */

/* These are defined in C code. */
	.extern SYM (main)
	.extern SYM (exit)
	.extern SYM (atexit)
	.extern SYM(__do_global_dtors)

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

    
	move.l  4(sp),d0	/* Get argv from stack */
	move.l  8(sp),d1	/* Get argc from stack */

	move.l d1,-(sp)
	move.l d0,-(sp)

	PICCALL	SYM (main)
	lea (8,sp),sp

	rts	

