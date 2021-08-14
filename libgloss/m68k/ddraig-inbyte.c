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
