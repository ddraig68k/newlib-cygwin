/*
 * ddraig_pie.ld — position-independent executable linker script for DdraigOS.
 *
 * Differences from ddraig.ld:
 *   - ORIGIN = 0 (OS loader chooses the actual load address)
 *   - Explicit PHDRS block: PT_LOAD(RX) + PT_LOAD(RW) + PT_DYNAMIC.
 *     No PT_PHDR — the auto-generated PT_PHDR caused "PHDR segment not
 *     covered by LOAD segment" because .text starts at vaddr 0 with no
 *     room reserved for the ELF header/PHDRs. The OS loader does not need
 *     PT_PHDR; it locates RELA via PT_DYNAMIC only.
 *   - Explicit PT_DYNAMIC so the linker emits the dynamic section even
 *     with a PHDRS block (a PHDRS block suppresses auto-generation of any
 *     header not listed, including PT_DYNAMIC).
 *   - .rela.dyn captured explicitly in the RX segment so the OS loader
 *     can reach it via the DT_RELA virtual address.
 *
 * Build with: m68k-ddraig-elf-ld --pie -T ddraig_pie.ld ...
 * Do NOT add -fpic — it switches to the A5-GOT model which requires a
 * dynamic linker to set up A5. Plain --pie generates R_68K_RELATIVE and
 * R_68K_32 RELA entries which the OS loader applies at load time.
 */

STARTUP(ddraig_crt0.o)
OUTPUT_ARCH(m68k)

SEARCH_DIR(.)
GROUP(-lddraig -lc -lgcc)
__DYNAMIC  =  0;

MEMORY
{
  ram (rwx) : ORIGIN = 0, LENGTH = 8M-0x2000
}

PROVIDE (__stack = 0);

PROVIDE (hardware_init_hook = 0);
PROVIDE (_hardware_init_hook = 0);
PROVIDE (software_init_hook = 0);
PROVIDE (_software_init_hook = 0);

PHDRS
{
    text    PT_LOAD FLAGS(5);   /* R + X */
    data    PT_LOAD FLAGS(6);   /* R + W */
    dynamic PT_DYNAMIC;
}

SECTIONS
{
  .text :
  {
    CREATE_OBJECT_SYMBOLS
    *(.text .text.*)

    . = ALIGN(0x4);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))

    *(.rodata .rodata.*)

    . = ALIGN(0x4);
    *(.gcc_except_table)

    . = ALIGN(0x4);
    *(.eh_frame)

    . = ALIGN(0x4);
    __INIT_SECTION__ = . ;
    LONG (0x4e560000)   /* linkw %fp,#0 */
    *(.init)
    SHORT (0x4e5e)      /* unlk %fp */
    SHORT (0x4e75)      /* rts */

    . = ALIGN(0x4);
    __FINI_SECTION__ = . ;
    LONG (0x4e560000)   /* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4e5e)      /* unlk %fp */
    SHORT (0x4e75)      /* rts */

    . = ALIGN(0x4);
    _etext = .;
    *(.lit)
  } :text

  .rela.dyn : ALIGN(4) { *(.rela .rela.*) } :text

  .data :
  {
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.*)
    _edata = .;
  } :data

  .dynamic : { *(.dynamic) } :data :dynamic

  .bss :
  {
    . = ALIGN(0x4);
    __bss_start = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(COMMON)
    _end =  ALIGN (0x8);
    __end = _end;
  } :data

  .stab 0 (NOLOAD) : { *(.stab) }
  .stabstr 0 (NOLOAD) : { *(.stabstr) }
}
