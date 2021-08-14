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
 * outbyte -- send BYTE out the DUART's channel A, for display to
 *      the user.
 *
 *      Normally, this is identical to raw_outbyte, but if
 *      GDB_MONITOR_OUTPUT is #defined, we prefix each byte we send
 *      with a ^O character (ASCII 15).  This is a signal to GDB's
 *      `rom68k' target to pass the character directly on to the user;
 *      it allows programs to do console output under GDB.
 *
 *      We compile this file twice: once with GDB_MONITOR_OUTPUT
 *      #defined, and once without.  The former .o file we put in
 *      libidpgdb.a, which is included in the link by idpgdb.ld; the
 *      latter we put in libidp.a, which is selected by idp.ld.
 */
void outbyte (char data)
{
    volatile char status = DUART_READ(DUART_SRA);
    while ((status & TxRDY) != TxRDY)
    {
        // Wait until the transmit buffer is empty
        status = DUART_READ(DUART_SRA);
    }

    DUART_WRITE(DUART_TBA, data);
}
