/* idp-inbyte.c --
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <_ansi.h>

#define DUART_BASE      0x00F7F000       // Base address of the Dual Asynchronous Receiver Transmitter
#define DUART_READ(x)      (*((volatile char *) DUART_BASE + x))
#define DUART_WRITE(x, y)  (*((char *) DUART_BASE + x) = y)
#define DUART_SRA       0x02             // Status Register A
#define DUART_RBA       0x06             // Receive Buffer A
#define DUART_TBA       0x06             // Transmit Buffer A
// Status Register flags
#define RxRDY		    0x01
#define TxRDY		    0x04

/*
 * inbyte -- get a byte from the DUART RX buffer. This only reads
 *           from channel A
 */
char inbyte(void)
{
  while ((DUART_READ(DUART_SRA) & 0x01) == 0x00)
    ;

  return DUART_READ(DUART_RBA); /* read the byte */
}
